import React, { useState, useEffect } from 'react';
import mqtt from 'mqtt';
import '../App.css'; 

const Termometro = () => {
  const [temperatura, setTemperatura] = useState('--');
  const [umidade, setUmidade] = useState('--');
  const [status, setStatus] = useState('Desconectado');

  useEffect(() => {
    const client = mqtt.connect('ws://broker.hivemq.com:8000/mqtt');

    client.on('connect', () => {
      setStatus('Conectado');
      client.subscribe('quarto/temperatura');
      client.subscribe('quarto/umidade');
    });

    client.on('message', (topic, message) => {
      const valor = message.toString();
      if (topic === 'quarto/temperatura') setTemperatura(valor);
      if (topic === 'quarto/umidade') setUmidade(valor);
    });

    client.on('offline', () => {
      setStatus('Desconectado');
    });

    return () => {
      if (client.connected) client.end();
    };
  }, []);

  return (
    <div className="dashboard-container">
      <h1 className="dashboard-title">Painel IoT • Quarto</h1>
      
      {/* Lógica dinâmica: Se status for 'Conectado', usa a classe verde, senão vermelha */}
      <div className={`status-badge ${status === 'Conectado' ? 'status-online' : 'status-offline'}`}>
        {status}
      </div>
      
      <div className="cards-grid">
        {/* Card Temperatura */}
        <div className="card card-temp">
          <h2>Temperatura</h2>
          <div className="card-value">
            {temperatura}<span className="unit">°C</span>
          </div>
        </div>

        {/* Card Umidade */}
        <div className="card card-umid">
          <h2>Umidade</h2>
          <div className="card-value">
            {umidade}<span className="unit">%</span>
          </div>
        </div>
      </div>
    </div>
  );
};

export default Termometro;