import React, { useEffect, useState } from "react";
import { database } from "./firebase";

const PatientData = () => {
  const [patientData, setPatientData] = useState({});

  useEffect(() => {
    database.ref("patient").on("value", (snapshot) => {
      if (snapshot.exists()) {
        setPatientData(snapshot.val());
      }
    });
  }, []);

  return (
    <div className="p-4 bg-gray-100">
      <h2 className="text-xl font-bold">Patient Data</h2>
      <p>Temperature: {patientData.temperature}°C</p>
      <p>Heart Rate: {patientData.heart_rate} BPM</p>
    </div>
  );
};

export default PatientData;
