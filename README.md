# ESP8266 Projects - ENGCE123

โปรเจคนี้ประกอบด้วยการทดลองต่างๆ สำหรับ ESP8266 ในวิชา ENGCE123 (embedded system) แบ่งออกเป็น 2 LAB หลัก

## 📋 สารบัญ

- [LAB 2: Bootstrap Web Server](#lab-2-bootstrap-web-server)
- [LAB 3: WiFi Manager](#lab-3-wifi-manager)

## 🌐 LAB 2: Bootstrap Web Server

### คำอธิบาย
Web Server บน ESP8266 ที่ใช้ Bootstrap สำหรับสร้าง UI ในการควบคุมเวลาเปิด-ปิดอุปกรณ์ (Time Control System)

### ฟีเจอร์หลัก
- ✅ เชื่อมต่อ WiFi อัตโนมัติ
- ✅ Web Interface ที่สวยงามด้วย Bootstrap 4
- ✅ ตั้งเวลาเปิด-ปิดอุปกรณ์หลายช่วงเวลา
- ✅ ระบบ NTP สำหรับซิงค์เวลา
- ✅ ควบคุม LED ตามเวลาที่กำหนด

### องค์ประกอบ
- **Time Table**: แสดงรายการเวลาที่ตั้งไว้
- **Time Setting Form**: ฟอร์มสำหรับเพิ่มเวลาใหม่
- **Order System**: ลำดับการทำงาน
- **Release Duration**: ระยะเวลาเปิดอุปกรณ์ (วินาที)
- **Active Status**: สถานะเปิด/ปิดการทำงาน

### การตั้งค่า WiFi
```cpp
const char* ssid = "papon";
const char* password = "0633468203";
```

### API Endpoints
- `GET /` - หน้าหลัก Web Interface
- `POST /set_time` - ตั้งค่าเวลาใหม่

## 🔧 LAB 3: WiFi Manager

โปรเจคนี้แบ่งออกเป็น 2 ส่วน:

### 3.1 WiFi Manager Basic

#### คำอธิบาย
ระบบจัดการ WiFi ที่ไม่ต้องฮาร์ดโค้ด SSID และ Password

#### ฟีเจอร์
- ✅ สร้าง Access Point เมื่อไม่สามารถเชื่อมต่อ WiFi ได้
- ✅ Portal สำหรับตั้งค่า WiFi ผ่าน Web Browser
- ✅ Reset การตั้งค่าด้วยปุ่ม Flash
- ✅ เชื่อมต่ออัตโนมัติเมื่อบูต

#### การใช้งาน
1. เมื่อ ESP8266 ไม่สามารถเชื่อมต่อ WiFi ได้ จะสร้าง AP ชื่อ "MyESP8266AP"
2. เชื่อมต่อกับ AP นี้แล้วเปิดเว็บบราวเซอร์
3. ตั้งค่า WiFi ที่ต้องการเชื่อมต่อ
4. กดปุ่ม Flash ค้างไว้เพื่อ Reset การตั้งค่า

### 3.2 WiFi Manager + LINE Notify + Web Server

#### คำอธิบาย
ขยายจาก WiFi Manager Basic โดยเพิ่ม LINE Notify และ Web Server สำหรับควบคุม LED

#### ฟีเจอร์เพิ่มเติม
- 📱 แจ้งเตือนผ่าน LINE Notify เมื่อเชื่อมต่อ WiFi สำเร็จ
- 💡 ควบคุม LED ผ่าน Web Interface
- 📡 แจ้งเตือนสถานะ LED ผ่าน LINE

#### LINE Token
```cpp
const char* lineToken = "DXvQXNtlcGv5qX6XR60hYYrhCDp1EeGSEsG9fS0OyHY";
```

#### API Endpoints
- `GET /` - หน้าหลักควบคุม LED
- `GET /led/on` - เปิด LED
- `GET /led/off` - ปิด LED

## 🛠 การติดตั้ง

### ความต้องการ
- ESP8266 Development Board
- Arduino IDE หรือ PlatformIO
- WiFi Network

### Libraries ที่ต้องติดตั้ง
```cpp
// สำหรับทุกโปรเจค
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// สำหรับ LAB 2
#include <WiFiUdp.h>
#include <NTPClient.h>

// สำหรับ LAB 3
#include <DNSServer.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
```

### วิธีติดตั้ง Libraries
1. เปิด Arduino IDE
2. ไป Tools → Manage Libraries
3. ค้นหาและติดตั้ง:
   - `WiFiManager by tzapu`
   - `NTPClient by Fabrice Weinberg`

## 📖 การใช้งาน

### LAB 2: Bootstrap Web Server
1. อัพโหลดโค้ดลง ESP8266
2. เปิด Serial Monitor ดู IP Address
3. เปิดเว็บบราวเซอร์ไปที่ IP Address
4. ใช้งานผ่าน Web Interface

### LAB 3.1: WiFi Manager
1. อัพโหลดโค้ดลง ESP8266
2. ถ้าไม่มี WiFi ที่บันทึกไว้ จะเห็น AP "MyESP8266AP"
3. เชื่อมต่อกับ AP และตั้งค่า WiFi

### LAB 3.2: WiFi Manager + LINE Notify
1. สร้าง LINE Notify Token จาก https://notify-bot.line.me/
2. แทนที่ token ในโค้ด
3. อัพโหลดโค้ดลง ESP8266
4. ตั้งค่า WiFi (ถ้าจำเป็น)
5. ใช้งานผ่าน Web Interface และรับแจ้งเตือนใน LINE

## 🔌 การเชื่อมต่อฮาร์ดแวร์

### ขา GPIO ที่ใช้
- **Flash Button**: GPIO 0 (สำหรับ Reset WiFi Settings)
- **Built-in LED**: GPIO 2 (สำหรับการควบคุม)

## 🚀 การพัฒนาต่อ

### แนวคิดสำหรับการขยายโปรเจค
- เพิ่ม Sensor ต่างๆ (อุณหภูมิ, ความชื้น)
- ควบคุมอุปกรณ์ไฟฟ้าผ่าน Relay
- บันทึกข้อมูลใน Database
- สร้าง Mobile Application
- เพิ่มการแจ้งเตือนผ่าน Discord, Slack

## 📝 หมายเหตุ

- ตรวจสอบว่า ESP8266 เชื่อมต่ออินเทอร์เน็ตได้สำหรับ NTP และ LINE Notify
- LED_BUILTIN เป็น Active Low (LOW = เปิด, HIGH = ปิด)
- สำหรับ LAB 2 ใช้ NTP Offset 25200 วินาที (UTC+7 สำหรับประเทศไทย)

## 📄 License

โปรเจคนี้ใช้สำหรับการศึกษาในวิชา ENGCE123

---

**ผู้จัดทำ**: ปพน แซ่จ๊ะ 
**วันที่ปรับปรุงล่าสุด**: August 2025
