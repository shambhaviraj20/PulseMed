import React, { useEffect, useState } from "react";
import axios from "axios";
import { database } from "./firebase";

const Diagnosis = () => {
  const [diagnosis, setDiagnosis] = useState("");
  const [patientData, setPatientData] = useState({});

  useEffect(() => {
    const fetchPatientData = async () => {
      database.ref("patient").on("value", (snapshot) => {
        if (snapshot.exists()) {
          const data = snapshot.val();
          setPatientData(data);

          axios.post("http://127.0.0.1:5000/predict", data)
            .then(response => setDiagnosis(response.data.diagnosis))
            .catch(error => console.error("Error fetching diagnosis", error));
        }
      });
    };
    fetchPatientData();
  }, []);

  return (
    <div className="p-4 bg-gray-100">
      <h2 className="text-xl font-bold">AI Diagnosis</h2>
      <p className="text-red-500 font-bold">{diagnosis}</p>
    </div>
  );
};

export default Diagnosis;
