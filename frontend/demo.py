import os
from dotenv import load_dotenv
import firebase_admin
from firebase_admin import credentials, db
import speech_recognition as sr
from deep_translator import GoogleTranslator  
from flask import Flask, request, jsonify
from flask_cors import CORS

# LangChain & AI Imports
from langchain.text_splitter import RecursiveCharacterTextSplitter
from langchain_huggingface import HuggingFaceEmbeddings
from langchain_pinecone import PineconeVectorStore
from langchain.chains import create_retrieval_chain
from langchain.chains.combine_documents import create_stuff_documents_chain
from langchain_core.prompts import ChatPromptTemplate
from pinecone import Pinecone, ServerlessSpec  
from langchain_openai import ChatOpenAI  
from sentence_transformers import SentenceTransformer

# ------------------------- Load .env Variables -------------------------
load_dotenv()

OPENAI_API_KEY = os.getenv("OPENAI_API_KEY")
PINECONE_API_KEY = os.getenv("PINECONE_API_KEY")
PINECONE_INDEX_NAME = os.getenv("PINECONE_INDEX_NAME")
HUGGINGFACE_MODEL = os.getenv("HUGGINGFACE_MODEL")
FIREBASE_CREDENTIALS_PATH = os.getenv("FIREBASE_CREDENTIALS_PATH")
FIREBASE_DATABASE_URL = os.getenv("FIREBASE_DATABASE_URL")

# ------------------------- Initialize Pinecone -------------------------
def initialize_embeddings():
    return HuggingFaceEmbeddings(model_name=HUGGINGFACE_MODEL)

embeddings = initialize_embeddings()
model = SentenceTransformer(HUGGINGFACE_MODEL)

# ------------------------- Connect to Pinecone -------------------------
docsearch = PineconeVectorStore.from_existing_index(
    index_name=PINECONE_INDEX_NAME,
    embedding=embeddings,
)
retriever = docsearch.as_retriever(search_type="similarity", search_kwargs={"k": 3})

# ------------------------- Setup LLM (GPT-4) -------------------------
llm = ChatOpenAI(model="gpt-4o-mini", temperature=0.4, max_tokens=100)

system_prompt = (
    "You are an assistant for question-answering tasks. "
    "Use the following pieces of retrieved context to answer "
    "the question. If you don't know the answer, say that you "
    "don't know. Use three sentences maximum and keep the "
    "answer concise."
    "\n\n"
    "{context}"
)

prompt = ChatPromptTemplate.from_messages(
    [
        ("system", system_prompt),
        ("human", "{input}"),
    ]
)

question_answer_chain = create_stuff_documents_chain(llm, prompt)
rag_chain = create_retrieval_chain(retriever, question_answer_chain)

# ------------------------- Firebase Setup -------------------------
def initialize_firebase():
    if not firebase_admin._apps:
        cred = credentials.Certificate(FIREBASE_CREDENTIALS_PATH)
        firebase_admin.initialize_app(cred, {
            'databaseURL': FIREBASE_DATABASE_URL
        })

def get_patient_data():
    ref = db.reference('patients')
    data = ref.get()
    return data.get('SOS', 'No SOS Message'), data.get('avgHeartRate', 'No Heart Rate Data'), data.get('avgTemperature', 'No Temperature Data')

# ------------------------- Flask Setup -------------------------
app = Flask(__name__)
CORS(app)

@app.route("/diagnose", methods=["POST"])
def diagnose():
    data = request.get_json()
    user_input = data.get("input", "No input provided")

    sos, avgHeartRate, avgTemperature = get_patient_data()
    final_prompt = f"{user_input} what immediate medications should be taken? My Heart Rate is {avgHeartRate} bpm and body temperature {avgTemperature} °C."
    
    response = rag_chain.invoke({"input": final_prompt})
    diagnosis = response["answer"]

    # Store in Firebase
    diagnosis_ref = db.reference("patients/diagnosis")
    diagnosis_ref.set({"diagnosis": diagnosis})

    return jsonify({"diagnosis": diagnosis})

@app.route("/get-diagnosis", methods=["GET"])
def get_diagnosis():
    diagnosis_ref = db.reference("patients/diagnosis")
    diagnosis_data = diagnosis_ref.get()
    return jsonify(diagnosis_data or {"diagnosis": "No diagnosis available"})

@app.route("/voice-recognition", methods=["POST"])
def voice_recognition():
    recognizer = sr.Recognizer()
    with sr.Microphone() as source:
        recognizer.adjust_for_ambient_noise(source)
        try:
            audio = recognizer.listen(source)
            detected_text = recognizer.recognize_google(audio, language="hi-IN")
            translated_text = GoogleTranslator(source='auto', target='en').translate(detected_text)
            return jsonify({"detected": detected_text, "translated": translated_text})
        except sr.UnknownValueError:
            return jsonify({"error": "Could not understand audio"})
        except sr.RequestError:
            return jsonify({"error": "Could not request results, please check your connection"})

@app.route("/patient-data", methods=["GET"])
def patient_data():
    sos, heart_rate, temperature = get_patient_data()
    return jsonify({"sos": sos, "heart_rate": avgHeartRate, "temperature": avgTemperature})

@app.route("/generate-response", methods=["POST"])
def generate_response():
    data = request.get_json()
    user_input = data.get("input", "No input provided")
    
    sos, avgHeartRate, avgTemperature = get_patient_data()
    final_prompt = f"{user_input} what immediate medications should be taken? My Heart Rate is {avgHeartRate} bpm and body temperature {avgTemperature} °C."
    
    response = rag_chain.invoke({"input": final_prompt})
    return jsonify({"response": response["answer"]})

@app.route("/speech-to-text", methods=["POST"])
def speech_to_text():
    return voice_recognition()

if __name__ == "__main__":
    initialize_firebase()
    app.run(host="0.0.0.0", port=5000, debug=True)