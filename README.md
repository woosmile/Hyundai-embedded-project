# MOVILINK: 차량 제어 플랫폼

MOVILINK은 차량 상태를 모니터링하고 원격으로 제어할 수 있는 차량 제어 플랫폼입니다.  
웹을 통해 차량의 다양한 기능을 제어할 수 있어 사용자에게 편리한 환경을 제공합니다.

## 📌 주요 기능

### 🚗 차량 제어 기능
웹을 통해 차량을 원격으로 제어할 수 있습니다.
- **차량 시동**: On/Off
- **차량 문**: 열림/닫힘
- **차량 선루프**: 열림/닫힘
- **에어컨**: On/Off(온도 및 습도에 따라 자동 제어 기능 제공)
- **차량 주행**: 출발/정지

### 📊 차량 모니터링 기능
차량의 상태를 실시간으로 모니터링할 수 있습니다.
- **차량 온도 및 습도**: 측정 및 표시
- **진동 감지 여부**: 감지 및 표시
- **차량 속도**: 측정 및 표시
- **스피커 알림**: 특정 동작 발생 시 (차량 시동, 문 & 선루프 개폐, 차량 주행/정지)
- **LCD 디스플레이 지원**: 차량 상태 시각화(차량 시동 여부, 온도, 습도, 차량 속도)

## 🎥 시연 영상

## 🛠 하드웨어 구성

### Sensor Module
#### Easy Module Shield
- 차량 상태를 감지하는 센서 모듈
- 온도 및 습도: DHT11
- 차량 속도: Potentiometer
#### SW-420
- 진동 감지 센서 모듈
#### Nucelo-L073RZ
- 측정된 센서값을 Analog to Digital Convert 후 LCD 및 Master 모듈로 전송

### LCD Module
#### LCD 1602
- 차량 상태 확인 LCD
#### Nucelo-L073RZ
- 차량 상태 데이터 LCD 출력 및 Master 모듈로 전송

### Master Module
#### Speaker
- 특정 동작 시 효과음 출력
#### 180° Servo Motor
- 차량 문, 선루프 개폐
#### 360° Servo Motor
- 차량 주행 출발/정지
#### Nucelo-L073RZ
- 센서 데이터 수신 및 차량 상태 관리
- ESP32와 통신하여 데이터를 웹 서버로 전송
- 웹에서 수신한 제어 명령에 따라서 출력 액추에이터 제어

### ESP32
- Master로부터 받은 차량 상태 데이터를 웹 서버로 전송
- 웹에서 받은 제어 명령을 Master로 전달

## 💻 소프트웨어 구성

### 🔧 임베디드 시스템
- **개발 언어**: C
- **IDE**: Keil uVision
- **통신 프로토콜**: UART, Wi-Fi
  
### 🌐 웹 서비스
- **개발 언어**: JavaScript
- **프레임워크**: React, Express.js, Node.js, MongoDB

## 📝 시스템 개발 설계서
![Image](https://github.com/user-attachments/assets/a50ad8e5-da95-4a9d-a6ee-40d2b3c59a7a)

### 센서 통신 데이터 포맷
- uint8_t 15Bit
- 000(온도) 000(습도) 000(조도) 000(진동감지) 000(가변저항, 속도)

### 차량 상태 통신 데이터 포맷
- uint8_t 31Bit
- 0(차량상태변화여부) 000(온도) 000(습도) 000(조도) 000(진동감지) 000(가변저항) 000(문) 000(에어컨) 000(선루프) 000(시동) 000(주행여부)

## 📝 네트워크 아키텍처
![Image](https://github.com/user-attachments/assets/c39b5394-a3bc-4a21-9180-72cde950c230)

## 🏗️ Diagram & Flow Chart
<a href="[https://github.com](https://drive.google.com/file/d/1Jk9-tknyK9WS17EQ_y63NOpmKbOjyJ9z/view?usp=sharing)" target="_blank">전체 Diagram & Flow Chart</a>

### Master Module Diagram
![Image](https://github.com/user-attachments/assets/309f9c15-96fc-469d-b278-12c671c6e387)

### Master Module Flow Chart
![Image](https://github.com/user-attachments/assets/d94fef61-37fb-48cc-89ea-5dd366291ff3)

## 📌 프로젝트 구조
```
Hyundai-embedded-project/
│── ESP32/          # ESP32 코드
│── lcd/            # LCD 코드
│── master/         # Master 코드
│── sensor/         # Sensor 코드
└── README.md       # 프로젝트 설명 파일
```

## 팀 구성원

| 이름 | GitHub |
|------|--------|
| 이상빈 | [@ideal402](https://github.com/ideal402) |
| 옥창희 | [@okchangheeok](https://github.com/okchangheeok) |
| 류우경 | [@lolololoop](https://github.com/lolololoop) |
| 김우성 | [@woosmile](https://github.com/woosmile) |
