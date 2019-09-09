
# MQTT Client API

MQTT version 3.1 client API libary for Mosquitto Broker in C.

![Version:1.0](https://img.shields.io/badge/Version-1.0-green.svg)
![In Development](https://img.shields.io/badge/Status-In%20Development-orange.svg)
![IDE:Eclipse](https://img.shields.io/badge/Ide-Eclipse-blue)
![Protoc(ol:MQTT](https://img.shields.io/badge/Protocol-MQTT-blue)
![License:MIT](https://img.shields.io/github/license/adimalla/MQTT-3.1-C?label=License)
&nbsp;
&nbsp;
![Last Commit:date](https://img.shields.io/github/last-commit/adimalla/MQTT-3.1-C?label=Last%20Commit&style=plastic)

## Description
The Goal of the project is to create an MQTT client API, portable in both POSIX and non POSIX embedded system enviornments. The APIs are minimal, easy to understand and independent of posix sockets and other socket based or networking APIs and are implemented in C for ease of portablity in resource constrained embedded devices.

## Features
Currently the APIs are tesed with C console applications under Linux enviornment using socket API and comes with publisher state machine example code that supports quality of service upto level 1 and message retention at MQTT broker.

## Porting and Instalation

### Linux
Currently there are no instalation scripts, however, in development code is available in API directory.

### Windows
NA

## Contributors and Maintainers
Aditya Mall                 (UTA MSEE)

