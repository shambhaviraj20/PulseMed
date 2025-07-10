import React from "react";
import PatientData from "./PatientData";   // Update the path
import Diagnosis from "./Diagnosis";       // Update the path

function App() {
  return (
    <div className="App">
      <h1 className="text-2xl font-bold">MedBox Platform</h1>
      <PatientData />
      <Diagnosis />
    </div>
  );
}

export default App;
