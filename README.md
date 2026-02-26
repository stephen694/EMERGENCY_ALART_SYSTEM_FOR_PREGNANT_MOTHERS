# Emergency Alert System for Pregnant Mothers in Kenya

> *Saving Lives with One Button — Fast, Simple, Reliable Maternal Emergency Alerts*

# Quick Links

- [Project Report (Final)](./Emergency_Alert_System_Final_Report.docx)
- [Circuit Schematic (Fritzing / EasyEDA)](./doc/schematic/)
- [Firmware Source Code](./src/)
- [Deployment & Field Testing Notes](./doc/deployment.md)

# Emergency Alert System on Arduino

This project brings a one-button maternal emergency alert device to life using the Arduino Uno platform. It allows a pregnant woman — with no smartphone, no internet, and no need to speak — to dispatch an SMS alert containing her real-time GPS location to pre-registered healthcare contacts with a single button press.

The system leverages Kenya's existing GSM mobile network infrastructure (Safaricom, Airtel, Telkom) and is designed to complement the [m-mama](https://www.mobilematernityafrica.org/) emergency transport system. Its low-power design supports both USB and solar charging, making it suitable for off-grid rural Kenyan communities.

# Contents

- Installation & Setup:
  - [Hardware Assembly](#hardware-assembly)
  - [Uploading Firmware via Arduino IDE](#uploading-firmware-via-arduino-ide)
  - [Configuring Emergency Contacts](#configuring-emergency-contacts)
  - [Power Options](#power-options)
- [How It Works](#how-it-works)
- [System Architecture](#system-architecture)
- [Components & Libraries](#components--libraries)
- [Documentation](#documentation)
- [Issues and Support](#issues-and-support)
- [Contributing](#contributing)
- [License and Credits](#license-and-credits)

---

## Hardware Assembly

The following components are required to assemble the device:

| Component | Specification |
|-----------|--------------|
| Microcontroller | Arduino Uno |
| GSM Module | SIM800L or SIM900 |
| GPS Module | Neo-6M |
| Display | 16x2 LCD |
| User Input | Physical Push Button |
| Audio Feedback | 5V Buzzer |
| Power | USB or Solar-compatible stable supply |

Refer to the circuit schematic in [`/doc/schematic/`](./doc/schematic/) for wiring diagrams produced in Fritzing and EasyEDA.

1. Wire the **Neo-6M GPS module** to the Arduino using SoftwareSerial pins (D2/D3).
2. Wire the **SIM800L/SIM900 GSM module** to the Arduino using SoftwareSerial pins (D7/D8). Ensure the GSM module has its own stable power supply (3.4–4.2V for SIM800L) to handle peak current draw.
3. Connect the **16x2 LCD** via I2C (SDA/SCL) or direct 4-bit parallel wiring to digital pins.
4. Connect the **push button** to digital pin D4 with a pull-down resistor.
5. Connect the **buzzer** to digital pin D5.

---

## Uploading Firmware via Arduino IDE

1. [Download and install the Arduino IDE](https://www.arduino.cc/en/software) (v1.x or v2.x).
2. Clone or download this repository.
3. Open `src/emergency_alert/emergency_alert.ino` in the Arduino IDE.
4. Install the required libraries via **Tools > Manage Libraries**:
   - **TinyGPS++** by Mikal Hart
   - **SoftwareSerial** (included with Arduino IDE)
   - **LiquidCrystal** or **LiquidCrystal_I2C** (depending on LCD wiring)
5. Select **Tools > Board > Arduino Uno**.
6. Select the correct COM port under **Tools > Port**.
7. Click **Upload**.

---

## Configuring Emergency Contacts

Pre-registered healthcare contact numbers are set in the firmware before upload. Open `src/emergency_alert/config.h` and edit the following:

```cpp
// Add up to 5 pre-registered emergency contact numbers
const char* EMERGENCY_CONTACTS[] = {
  "+254XXXXXXXXX",   // Primary healthcare contact
  "+254XXXXXXXXX",   // Secondary contact (midwife / CHW)
};
const int NUM_CONTACTS = 2;
```

The outgoing SMS will be sent to all listed numbers simultaneously.

---

## Power Options

The device supports two charging modes:

- **USB Charging** — Connect to any USB 5V power source or power bank.
- **Solar Charging** — A solar panel with a 5V regulator can be connected to the USB input for off-grid operation. Recommended for field deployment in rural settings without reliable grid access.

The LCD displays a low-battery warning when charge drops to critical levels.

---

## How It Works

1. The user **presses and holds** the emergency button for **3 seconds**.
2. The LCD prompts: *"Press again to confirm"* — preventing false activations.
3. The Arduino queries the **Neo-6M GPS module** for current coordinates.
   - GPS lock available → coordinates formatted as a **Google Maps URL**.
   - GPS lock unavailable → last known coordinates used, or *"Location Unavailable"* noted in SMS.
4. The **SIM800L/SIM900 GSM module** transmits the SMS to all pre-registered contacts.
5. If delivery fails, **automatic retry logic** re-attempts transmission.
6. The LCD displays: *"Alert Sent — Help Coming"* (English and Swahili).
7. The buzzer sounds for audible confirmation.

### SMS Format

```
EMERGENCY: Pregnant woman needs help.
Msaada: Mwanamke mjamzito anahitaji msaada.
Location: https://maps.google.com/?q=-1.2345,36.8765
Time: 2025-06-01 14:32:07
```

---

## System Architecture

```
[Button Hold 3s]
      │
      ▼
[Arduino Uno] ──► [Neo-6M GPS Module] ──► Fetch coordinates
      │
      ├──► Format Google Maps URL
      │
      ├──► [SIM800L/SIM900 GSM Module] ──► Send SMS → pre-registered contacts
      │                                        │
      │                                   [Retry if undelivered]
      │
      └──► [16x2 LCD + Buzzer] ──► Confirm to user
```

---

## Components & Libraries

### Hardware

| Component | Purpose |
|-----------|---------|
| Arduino Uno | Main microcontroller — coordinates all operations |
| SIM800L / SIM900 | SMS transmission over GSM without internet |
| Neo-6M GPS | Real-time geographic coordinate capture |
| Push Button | Single user input; 3s hold-to-confirm |
| 16x2 LCD | Confirmation messages (English & Swahili) |
| Buzzer | Audible alert confirmation |
| Stable Power Supply | USB + solar compatible; low-power design |

### Software & Libraries

| Library / Tool | Purpose |
|----------------|---------|
| Arduino IDE | Firmware development and upload |
| TinyGPS++ | Parses NMEA sentences from Neo-6M GPS |
| SoftwareSerial | Serial comms between Arduino, GPS, and GSM modules |
| LiquidCrystal / LiquidCrystal_I2C | LCD display control |
| Google Maps URL | Formats GPS coordinates as a clickable link in SMS |
| Fritzing / EasyEDA | Circuit schematic and PCB layout documentation |

---

## Documentation

Full project documentation is located in the [`/doc`](./doc/) directory and includes:

- Circuit schematics and PCB layout files
- Component datasheets
- Deployment field notes and testing logs
- User manual (English and Swahili)

The final project report covering problem statement, risk analysis, competitive landscape, and future work is available at the root of this repository: [`Emergency_Alert_System_Final_Report.docx`](./Emergency_Alert_System_Final_Report.docx).

---

## Issues and Support

If you encounter a bug or want to propose an enhancement, please [open an issue](../../issues) on GitHub.

When submitting an issue, please include:

- Arduino IDE version and board settings
- Sketch code (wrapped in a code block)
- Serial Monitor output at the time of the problem
- GSM module model and SIM network (Safaricom / Airtel / Telkom)
- GPS module model and whether testing was indoors or outdoors
- Battery/power supply configuration

For general questions about hardware assembly, deployment in the field, or community health worker training, please open a [Discussion](../../discussions).

---

## Contributing

For minor fixes to code or documentation, go ahead and submit a pull request.

For larger changes — new features, sensor additions, PCB redesigns, or new language support on the LCD — please open an issue first to discuss the approach before submitting a PR.

This project uses a **feature-branch workflow**:

- Work on a dedicated branch: e.g., `feature/solar-charging`, `feature/swahili-lcd`
- Submit a pull request for peer review before merging into `main`
- `main` always contains stable, tested code
- Use `hotfix/` branches for urgent bug fixes found during field testing
- Please squash small or iterative commits before opening a pull request

---

## License and Credits

This project is licensed under the [MIT License](./LICENSE).

**Project Team:**

| Name | Role |
|------|------|
| Stephen Ngige | Firmware development, Proteus simulation, debugging |
| Ham Thoya | Component selection, electrical design, SMS verification, performance validation |
| Julius Mwangi | Mechanical design, technical documentation |

**Third-Party Libraries:**

- [TinyGPS++](https://github.com/mikalhart/TinyGPSPlus) by Mikal Hart — licensed under LGPL v2.1
- [SoftwareSerial](https://github.com/plerup/espsoftwareserial) — included with the Arduino IDE
- [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C) by Frank de Brabander — licensed under GPL v3

**Related Projects & Inspiration:**

- [m-mama Emergency Transport System](https://www.mobilematernityafrica.org/) — complementary dispatch infrastructure this device is designed to integrate with
- [Arduino](https://www.arduino.cc/) — the open-source hardware and software platform this project is built on

---

*Designed specifically for the Kenyan context — leveraging existing GSM infrastructure to address a life-critical communication gap for pregnant women in rural and semi-urban Kenya.*
