<h1 align="center">🩺 PulseMed+</h1>
  
**AI-powered Rural Healthcare Assistant for Smart Diagnosis & SOS Alerts**

PulseMed+ bridges the healthcare accessibility gap in rural areas by combining AI-driven diagnosis, vitals monitoring, speech input, and emergency SMS alerts — all integrated with Firebase and Pinecone for real-time decision-making.

---

## 🚀 Features

### 🧠 AI Symptom Checker
- Uses **LangChain + OpenAI (GPT-4o-mini)** to analyze patient symptoms and vitals
- Returns short, precise medical suggestions (based on chatbot prompt chain)

### 🧾 PDF Medical Knowledge Loader
- Loads and chunks pages from `Harrison’s Principles of Internal Medicine`
- Uses `sentence-transformers` + Pinecone to create a **contextual vector search**

### 🗣 Hindi Voice Input
- Patients can **speak symptoms in Hindi**
- Translated via `deep-translator` and processed by AI

### 🔥 Live Vitals Input
- Fetches patient **temperature** and **heart rate** from Firebase Realtime DB
- Combines with symptom input for more accurate responses

### 📤 SOS Alert System
- Automatically sends SMS alerts to medical authorities using **Twilio**
- Message includes translated symptoms, vitals, and AI suggestions

---

## 🏗️ Project Architecture

### 🔹 Root Folder
- `README.md` — Project overview and documentation  
- `.gitignore` — Ignore list (includes `.env`, cache, node_modules, etc.)  
- `requirements.txt` — Python dependencies  
- `FINAL_FINAL.ino` — Arduino sketch to collect vitals  
- `firebase2.py` — Firebase client script  
- `Chatbot` — Chat UI logic or placeholder (rename if needed)

---

### 🔹 ML/ (Backend & LangChain)
- `app.ipynb` — Main LangChain chatbot (with Pinecone + OpenAI)  
- `sos_msg.ipynb` — Fetches vitals & sends SOS alerts via Twilio  
- `prompt.ipynb` — Prompt engineering experiments  
- `Libraries.ipynb` — Library setup & testing

---

### 🔹 frontend/ (React UI)
- `public/` — HTML, favicon, manifest  
- `src/` — React source code:
  - `App.js` — Main logic  
  - `Diagnosis.js` — AI-based medical response  
  - `PatientData.js` — Symptom & vitals form  
  - `firebase.js` — Firebase config  
  - `App.css`, `index.js`, etc.

---

📦 Organized for clarity:  
- 🧠 AI diagnosis (ML/)  
- 🎛️ React interface (frontend/)  
- ⚡ Real-time sensors + alerts (`.ino`, Firebase, Twilio)

---

## 📦 Tech Stack

| Layer        | Technology                          |
|--------------|--------------------------------------|
| LLM & RAG     | OpenAI GPT-4o-mini, LangChain        |
| Embeddings   | sentence-transformers (HuggingFace) |
| Vector DB    | Pinecone                             |
| Translation  | Deep Translator (`Hindi → English`)  |
| Firebase     | Realtime Database                    |
| Alerts       | Twilio SMS                           |
| UI Frontend  | React                 |
| Voice Input  | SpeechRecognition + Google API       |

---

## 🛠️ Local Setup

1. **Clone the repository**
   ```bash
   git clone git@github.com:shambhaviraj20/PulseMed.git
   cd PulseMed
2. **Create a .env file in root**
  - OPENAI_API_KEY=sk-...
  - TWILIO_ACCOUNT_SID=AC...
  - TWILIO_AUTH_TOKEN=...
  - PINECONE_API_KEY=pcsk-...
  - PINECONE_ENVIRONMENT=us-east-1
  - PINECONE_INDEX_NAME=medbot
4. **Install dependencies**
   pip install
5. **Run the system**
- Backend notebooks via Jupyter or Python
- Frontend with: npm start

---
## 📣 Team & Credits
- **Team Members:** Shambhavi Raj, Shravya Bhandary, Shardul Bangale, Sarthak Patil, Prasanna Tupe, Vishwanath Mishra
- **Achievement:** Team Meraki Titans - BITS APOGEE '25 Finalists

---

## 📌 Future Scope
- Patient-facing mobile app
- Doctor dashboard for remote diagnosis
- Integration with government e-health systems
- Use of MedGPT/ClinicalBERT for domain-specific enhancement

---

## 🛡️ License

This project is under the MIT License. Use freely, credit responsibly.

<br>
<p align="center">
  <sub><strong>Made with 💫 by Shambhavi</strong></sub><br>
  <sub><a href="https://github.com/shambhaviraj20">GitHub</a> • <a href="https://linkedin.com/in/shambhaviraj">LinkedIn</a> • <a href="https://shambhavirajportfolio.framer.website">Portfolio</a></sub><br>
  <sub><strong>This project was built with ❤️ to solve real-world healthcare accessibility challenges using AI and IoT.</strong></sub>
</p>

