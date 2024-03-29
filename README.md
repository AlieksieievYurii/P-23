# Tank P-23
[![p-23-ci](https://github.com/AlieksieievYurii/Tank/actions/workflows/main.yml/badge.svg)](https://github.com/AlieksieievYurii/Tank/actions/workflows/main.yml)

When I was a child, I always wanted to build a tank :D However, I didn't have the required skills for that. When I grew up,
I remembered that "childhood dream" and decided to try to build a tank. The main idea is to create a fully functional military RC
tank that can shoot and has a loading mechanism. Besides that, the platform should be multifunctional so that I can add
different modules like another turret or arm.

Features:
- Arduino Based Project (2x Mega, 3x Pro Mini)
- 4 cameras: front, back, commander, gunner
- Lights: back(Red), front, long front, gun, turret
- Laser
- Automatic Loading Mechanism
- Commander Turret
- Loading Drum can contain 6 shells
- Gun angle: -10..15 degrees
- Can shoot
- Radio control using nrf24l01
- Special Panel Control
- First Person View

## Video

[![image](https://github.com/AlieksieievYurii/P-23/assets/39415360/4de5bbf2-7b9b-404e-b98d-c6bb9d57bedb)](https://www.youtube.com/watch?v=BsqAmx8mWu0)

# Render
![Tank Platform 1](https://github.com/AlieksieievYurii/P-23/assets/39415360/aa100399-352b-4e76-bb74-02f8acf59fc8)
![Tank Platform 2](https://github.com/AlieksieievYurii/P-23/assets/39415360/e697cb0c-ef1e-459d-8845-2fcfade69cf9)
![Tank Platform 3](https://github.com/AlieksieievYurii/P-23/assets/39415360/0c03724d-af4b-43fe-aa4b-5b64b1bab898)


# Modules Architecture Overview

![p-23-modules-architecture drawio](https://github.com/AlieksieievYurii/P-23/assets/39415360/693e9efb-c09d-47fd-a3eb-a4c00fb6d0d7)


# Project Structure

* `docs` - contains different documents.
* `platform` - the folder contains sources and docs regarding the platform itself.
* `remote-panel-control` - the folder contains sources and docs regarding the radio control panel.
* `turret` - folder contains sources and docs of the turret.
* `index.html` - static web page containing information about donating of FPV drone for AFU.
* `res` - contains resources for the web page.
