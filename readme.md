# Projeto de Reconhecimento de Movimento com TinyML para ESP32

Este projeto implementa um **sistema de reconhecimento de movimento baseado em TinyML** usando um **ESP32**, o **acelerômetro HW-123** e **três LEDs** como atuadores. Padrões de movimento (cima/baixo, esquerda/direita, frente/trás) são detectados em tempo real por meio de um modelo Edge Impulse treinado. Quando um movimento é reconhecido, o ESP32 ativa o LED correspondente e publica o resultado em um tópico MQTT (`tinyml/movimento`).

---

## 1. Código-Fonte do ESP32

### 1.1 Documentação (Lógica do Sistema)
- O ESP32 lê continuamente dados do acelerômetro **HW-123** via I2C.
- Os valores de **accX, accY, accZ e steps** são organizados em janelas (buffers) conforme a parametrização do Edge Impulse.
- Cada janela é enviada ao **modelo TinyML** gerado pelo Edge Impulse.
- O modelo retorna uma classificação entre:
  - **cima_baixo**
  - **dir_esq**
  - **frente_tras**
- Dependendo da classe com maior confiança:
  - Acende-se um dos três LEDs conectados ao ESP32
  - Uma mensagem MQTT é publicada com o movimento detectado

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

### Confusion Matrix
<img width="623" height="256" alt="image" src="https://github.com/user-attachments/assets/7e3c6898-9934-4bac-b9e2-0e2c5290a994" />


### Accuracy e Loss
- **Accuracy:** 95.8%
- **Loss:** 0.11

### Parametrização da Janela
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
