/*
    Theengs - Decode things and devices
    Copyright: (c) Florian ROBERT

    This file is part of Theengs.

    Theengs is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Theengs is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEVICE_UTILS_THEENGS_H
#define DEVICE_UTILS_THEENGS_H
/* ************************************************************************** */

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QQmlContext>
#include <QQmlApplicationEngine>

/* ************************************************************************** */

class DeviceUtilsTheengs: public QObject
{
    Q_OBJECT

public:
    static void registerQML()
    {
        qRegisterMetaType<DeviceUtilsTheengs::DeviceSensorsTheengs>("DeviceUtilsTheengs::DeviceSensorsTheengs");
        qmlRegisterType<DeviceUtilsTheengs>("DeviceUtilsTheengs", 1, 0, "DeviceUtilsTheengs");
    }

    enum DeviceSensorsTheengs {
        // probes
        SENSOR_PROBES_TPMS          = (1 <<  0),
        SENSOR_TEMPERATURE_1        = (1 <<  1),
        SENSOR_TEMPERATURE_2        = (1 <<  2),
        SENSOR_TEMPERATURE_3        = (1 <<  3),
        SENSOR_TEMPERATURE_4        = (1 <<  4),
        SENSOR_TEMPERATURE_5        = (1 <<  5),
        SENSOR_TEMPERATURE_6        = (1 <<  6),

        // scales
        SENSOR_WEIGHT_UNIT          = (1 << 10),
        SENSOR_WEIGHT_MODE          = (1 << 11),
        SENSOR_WEIGHT               = (1 << 12),
        SENSOR_IMPEDANCE            = (1 << 13),

        // beacons
        SENSOR_ACCL_X               = (1 << 16), //!< Accelerometer
        SENSOR_ACCL_Y               = (1 << 17),
        SENSOR_ACCL_Z               = (1 << 18),

        SENSOR_GYRO_X               = (1 << 19), //!< Gyroscope
        SENSOR_GYRO_Y               = (1 << 20),
        SENSOR_GYRO_Z               = (1 << 21),

        // smartwatchs
        SENSOR_STEPS                = (1 << 22), //!< Pedometer
        SENSOR_HEARTRATE            = (1 << 23), //!< Heart rate monitor

        // others
        SENSOR_PRESENCE             = (1 << 24),
        SENSOR_MOVEMENT             = (1 << 25),
        SENSOR_OPEN                 = (1 << 26),
        SENSOR_ALARM                = (1 << 27),
        SENSOR_DISTANCE             = (1 << 28),

        // generic
        SENSOR_MODE                 = (1 << 30),
        SENSOR_STATE                = (1 << 31),
    };
    Q_ENUM(DeviceSensorsTheengs)

    enum DeviceTagsTheengs {
        TAG_THB     =  1,   //!< temperature, humidity, battery
        TAG_THBX    =  2,   //!< temperature, humidity, battery, extras
        TAG_BBQ     =  3,   //!< temperatures with several probes
        TAG_CTMO    =  4,   //!< contact and/or motion sensors
        TAG_SCALE   =  5,   //!< weight scales
        TAG_BCON    =  6,   //!< iBeacon protocol
        TAG_ACEL    =  7,   //!< acceleration
        TAG_BATT    =  8,   //!< battery
        TAG_PLANT   =  9,   //!< plant sensors
        TAG_TIRE    = 10,   //!< tire pressure monitoring system
        TAG_BODY    = 11,   //!< health monitoring devices
        TAG_ENRG    = 12,   //!< energy monitoring devices
        TAG_WCVR    = 13,   //!< window covering devices
        TAG_ACTR    = 14,   //!< ON/OFF actuators
        TAG_AIR     = 15,   //!< air environmental monitoring devices
    };
    Q_ENUM(DeviceTagsTheengs)
};

/* ************************************************************************** */

//! List of classes available in Home Assistant
const QStringList availableHASSClasses = {
    "battery",
    "carbon_monoxide",
    "carbon_dioxide",
    "pm10",
    "pm25",
    "humidity",
    "illuminance",
    "signal_strength",
    "temperature",
    "timestamp",
    "pressure",
    "power",
    "current",
    "energy",
    "power_factor",
    "voltage"
};

//! List of units available in Home Assistant
const QStringList availableHASSUnits = {
    "W",
    "kW",
    "V",
    "kWh",
    "A",
    "W",
    "°C",
    "°F",
    "ms",
    "s",
    "hPa",
    "L",
    "kg",
    "lb",
    "µS/cm",
    "ppm",
    "μg/m³",
    "m³",
    "mg/m³",
    "m/s²",
    "lx",
    "Ω",
    "%",
    "bar",
    "bpm",
    "dB",
    "dBm",
    "B"
};

/* ************************************************************************** */
#endif // DEVICE_UTILS_THEENGS_H
