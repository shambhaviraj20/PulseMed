// Import only required Firebase modules (MODULAR IMPORTS)
import { initializeApp } from "firebase/app";
import { getAuth } from "firebase/auth";          // If using Authentication
import { getFirestore } from "firebase/firestore"; // If using Firestore
import { getDatabase } from "firebase/database";   // If using Realtime Database
import { getStorage } from "firebase/storage";     // If using Firebase Storage

// Firebase Configuration
const firebaseConfig = {
  apiKey: "AIzaSyDtT3iEt2OWJvI-TPoTkE8dtU_e2xv6Jcg",
  authDomain: "medico-256d5.firebaseapp.com",
  databaseURL: "https://medico-256d5-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "medico-256d5",
  storageBucket: "medico-256d5.appspot.com",
  messagingSenderId: "559351513665",
  appId: "1:559351513665:web:5671edd196547a68d23cc0",
  measurementId: "G-TPD5MP9CY6"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

// Initialize Services
const auth = getAuth(app);
const db = getDatabase(app); // Realtime Database
const firestore = getFirestore(app); // Firestore Database
const storage = getStorage(app); // Firebase Storage

export { app, auth, db, firestore, storage };
