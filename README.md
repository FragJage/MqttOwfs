[![Build Status](https://travis-ci.org/FragJage/MqttOwfs.svg?branch=master)](https://travis-ci.org/FragJage/MqttOwfs)
[![Build status](https://ci.appveyor.com/api/projects/status/tdsue1qduo31wa17?svg=true)](https://ci.appveyor.com/project/FragJage/mqttowfs)
[![Coverage Status](https://coveralls.io/repos/github/FragJage/MqttOwfs/badge.svg?branch=master&bust=2)](https://coveralls.io/github/FragJage/MqttOwfs?branch=master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/402042548f424edb97592d5e8eb30eeb)](https://www.codacy.com/app/FragJage/MqttOwfs?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=FragJage/MqttOwfs&amp;utm_campaign=Badge_Grade)

MqttOwfs
=========
Mqtt daemon to communicate with one wire device. 

Introduction
============
This daemon send Mqtt message for state device and receive Mqtt command for input device. 
MqttOwfs use owserver, the backend component of the OWFS 1-wire bus control system.
See http://owfs.org/ for more information on OWFS 1-wire bus control system. 

Features
========
 - Automatic detection of devices 
 - Supported device without configuration file DS2405, DS18S20, DS1920, DS2406, DS2407, DS2423, DS2450, DS1921, DS1822, DS2438, DS18B20, DS2408, DS2413, DS18B25
 - All devices are supported by simple configuration file
 - OWFS can be install to another server
 - Possibility to set temperature and pressure unit

Portability
===========
Unit tests passed successfully on :
 - Linux Ubuntu (CPU Intel Celeron)
 - Windows 8 (CPU Intel Core I7)
 - Linux on Travis plateform
 - Windows on Appveyor plateform
 - More plateform can be supported such as Raspberry pi

Licence
=======
MqttOwfs is free software : you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

MqttOwfs is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with MqttOwfs. If not, see http://www.gnu.org/licenses/.
