# ESP32 TinyML Motion Recognition Project

This project implements a **TinyML-powered motion recognition system** using an **ESP32**, the **HW-123 accelerometer**, and **three LEDs** as actuators. Motion patterns (cima/baixo, esquerda/direita, frente/trás) are detected in real-time through a trained Edge Impulse model. When a motion is recognized, the ESP32 activates the corresponding LED and publishes the result to an MQTT topic (`tinyml/movimento`).

---

## 1. Código-Fonte do ESP32

### 1.1 Documentação (Lógica do Sistema)
- O ESP32 lê continuamente dados do acelerômetro **HW-123 (ADXL345/MPU variante)** via I2C.
- Os valores de **accX, accY, accZ e steps** são organizados em janelas (buffers) conforme a parametrização do Edge Impulse.
- Cada janela é enviada ao **modelo TinyML** gerado pelo Edge Impulse.
- O modelo retorna uma classificação entre:
  - **cima_baixo**
  - **dir_esq**
  - **frente_tras**
- Dependendo da classe com maior confiança:
  - Acende-se um dos três LEDs conectados ao ESP32
  - Uma mensagem MQTT é publicada com o movimento detectado
- Prints na serial mostram:
  - Dados capturados
  - Saída do modelo
  - LED ativado
  - Mensagem MQTT enviada

### 1.2 Código Completo (Sensor + TinyML)
> *(Cole aqui o código completo do ESP32)*

### 1.3 Exemplos de Serial Print
```
[ACC] X: -0.12 Y: 0.98 Z: 0.15
[EI] Predição => Frente_Tras (96%)
[LED] LED verde ativado
[MQTT] tinyml/movimento => "frente_tras"
```
```
[ACC] X: 1.21 Y: 0.20 Z: -0.04
[EI] Predição => Dir_Esq (95%)
[LED] LED azul ativado
[MQTT] tinyml/movimento => "dir_esq"
```

---

## 2. Treinamento TinyML (Edge Impulse)

### 2.1 Confusion Matrix
> *(Inserir ou descrever a matriz de confusão mostrada no painel: 95.1% / 95.7% / 96.4% etc.)*

### 2.2 Accuracy e Loss
- **Accuracy:** 95.8%
- **Loss:** 0.11

### 2.3 Parametrização da Janela
- **Window Size:** 1000 ms
- **Window Increase (Stride):** 250 ms
- **Frequência de Amostragem:** 19 Hz
- **Axes usados:** accX, accY, accZ, steps
- **Feature Extractor:** FFT (Spectral Features)
- **Modelo:** MLP com camadas densas

---

## 3. Mensagens Publicadas (MQTT)

### 3.1 Estrutura e Clareza das Mensagens
O ESP32 publica mensagens no tópico:
```
tinyml/movimento
```
Com payloads JSON simples:
```
{
  "movimento": "cima_baixo",
  "confianca": 0.951
}
```
Ou payload reduzido:
```
"cima_baixo"
```
---

## 📦 Tecnologias Utilizadas
- **ESP32**
- **HW-123 acelerômetro**
- **Edge Impulse (TinyML)**
- **MQTT / Mosquitto**
- **C++ (Arduino)**

---

## 📌 Funcionalidades
- Reconhecimento de movimentos em tempo real
- Inferência TinyML diretamente no microcontrolador
- Feedback visual com LEDs
- Envio remoto de telemetria via MQTT

---

## 🛠 Como Reproduzir
1. Fazer upload do firmware no ESP32
2. Conectar acelerômetro HW-123 via I2C
3. Configurar LEDs nos pinos indicados
4. Configurar WiFi + Broker MQTT no código
5. Monitorar a serial e o tópico `tinyml/movimento`

---

## © Autor
Projeto desenvolvido por Nicolas.

