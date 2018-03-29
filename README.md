[![Build Status](https://travis-ci.org/FragJage/xPLOwfs.svg?branch=master)](https://travis-ci.org/FragJage/xPLOwfs)
[![Coverage Status](https://coveralls.io/repos/github/FragJage/xPLOwfs/badge.svg?branch=master&bust=1)](https://coveralls.io/github/FragJage/xPLOwfs?branch=master)
[![Coverity Scan Build](https://scan.coverity.com/projects/10801/badge.svg)](https://scan.coverity.com/projects/10801)
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
 - OWFS can be install on another server
 - It's possible to set temperature and pressure unit

Portability
===========
TODO
Unit tests passed successfully on :
 - Windows Seven (CPU Intel Celeron)
 - Linux Ubuntu (CPU Intel Atom)
 - Linux Raspian on Raspberry Pi (CPU ARM)
 - Linux FunPlug on NAS DNS-320 (CPU ARM)

For more informations
=====================
TODO
See documentation in doc/html/index.html

Licence
=======
MqttOwfs is free software : you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

MqttOwfs is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with MqttOwfs. If not, see http://www.gnu.org/licenses/.
