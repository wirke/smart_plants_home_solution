import { useState } from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import './App.css'

function App() {
  return (
    <>
      <h1>Podaci</h1>
      <div class="card">
          <div>Temperatura</div>
          <div id="temp" class="value">--</div>
      </div>
      <div class="card">
          <div>Vlažnost</div>
          <div id="vlaga" class="value">--</div>
      </div>
      <div class="card">
          <div>Doba dana</div>
          <div id="status" class="value">--</div>
      </div>
    </>
  )
}

const espIP = "http://192.168.0.101/data";

async function updateData() {
    try {
        const response = await fetch(espIP);
        const data = await response.json();
        
        document.getElementById('temp').innerText = data.temp + " °C";
        document.getElementById('vlaga').innerText = data.vlaga + " %";
        const statusElement = document.getElementById('status');

if (data.status === "NOC") {
	statusElement.innerText = "Noć";
} else {
	statusElement.innerText = "Dan";
}
    } catch (err) {
        console.log("Greška pri osvežavanju:", err);
    }
}

setInterval(updateData, 2000);

export default App