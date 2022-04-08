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

#include "DeviceManager.h"
#include "MqttManager.h"

#include "utils/utils_app.h"
#include "utils/utils_bits.h"

#include <decoder.h> // Theengs decoder
#include "device_theengs.h"

#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyConnectionParameters>

#include <QList>
#include <QDateTime>
#include <QDebug>

/* ************************************************************************** */

void DeviceManager::updateBleDevice(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields)
{
    //qDebug() << "updateBleDevice() " << info.name() << info.address();
    //qDebug() << "updateBleDevice() " << info.name() << info.address() /*<< info.deviceUuid()*/ << " updatedFields: " << updatedFields;

    bool status = false;
    Q_UNUSED(updatedFields)

    if (info.address().toString() == info.name().replace('-', ':')) return; // skip beacons

    for (auto d: qAsConst(m_devices_model->m_devices)) // KNOWN DEVICES ////////
    {
        Device *dd = qobject_cast<Device*>(d);

#if defined(Q_OS_MACOS) || defined(Q_OS_IOS)
        if (dd && dd->getAddress() == info.deviceUuid().toString())
#else
        if (dd && dd->getAddress() == info.address().toString())
#endif
        {
            const QList<quint16> &manufacturerIds = info.manufacturerIds();
            for (const auto id: manufacturerIds)
            {
                //qDebug() << info.name() << info.address() << Qt::hex
                //         << "ID" << id
                //         << "manufacturer data" << Qt::dec << info.manufacturerData(id).count() << Qt::hex
                //         << "bytes:" << info.manufacturerData(id).toHex();

                dd->parseAdvertisementData(info.manufacturerData(id));

                DynamicJsonDocument doc(512);
                doc["id"] = info.address().toString().toStdString();
                doc["name"] = info.name().toStdString();
                doc["manufacturerdata"] = QByteArray::number(endian_flip_16(id), 16).toStdString() + info.manufacturerData(id).toHex().toStdString();
                doc["rssi"] = info.rssi();

                TheengsDecoder a;
                JsonObject obj = doc.as<JsonObject>();

                if (a.decodeBLEJson(obj) >= 0)
                {
                    std::string output;
                    serializeJson(obj, output);
                    //qDebug() << "output:" << output.c_str();

                    DeviceTheengs *ddd = dynamic_cast<DeviceTheengs*>(dd);
                    if (ddd) ddd->parseAdvertisementTheengs(QString::fromStdString(output));

                    SettingsManager *sm = SettingsManager::getInstance();
                    MqttManager *mq = MqttManager::getInstance();
                    if (sm && mq)
                    {
                        QString topic = sm->getMqttTopicA() + "/" + sm->getMqttTopicB() + "/BTtoMQTT/";
                        topic += info.address().toString().remove(':');

                        mq->publish(topic, QString::fromStdString(output));
                    }

                    status = true;
                }
                else
                {
                    //std::string input;
                    //serializeJson(doc, input);
                    //qDebug() << "input :" << input.c_str();
                }
            }

            const QList<QBluetoothUuid> &serviceIds = info.serviceIds();
            for (const auto id: serviceIds)
            {
                //qDebug() << info.name() << info.address() << Qt::hex
                //         << "ID" << id
                //         << "service data" << Qt::dec << info.serviceData(id).count() << Qt::hex
                //         << "bytes:" << info.serviceData(id).toHex();

                dd->parseAdvertisementData(info.serviceData(id));

                DynamicJsonDocument doc(512);
                doc["id"] = info.address().toString().toStdString();
                doc["name"] = info.name().toStdString();
                doc["servicedata"] = info.serviceData(id).toHex().toStdString();
                doc["servicedatauuid"] = id.toString().toStdString();
                doc["rssi"] = info.rssi();

                JsonObject obj = doc.as<JsonObject>();

                TheengsDecoder dec;
                if (dec.decodeBLEJson(obj) >= 0)
                {
                    std::string output;
                    serializeJson(obj, output);
                    //qDebug() << "output:" << output.c_str();

                    DeviceTheengs *ddd = dynamic_cast<DeviceTheengs*>(dd);
                    if (ddd) ddd->parseAdvertisementTheengs(QString::fromStdString(output));

                    SettingsManager *sm = SettingsManager::getInstance();
                    MqttManager *mq = MqttManager::getInstance();
                    if (sm && mq)
                    {
                        QString topic = sm->getMqttTopicA() + "/" + sm->getMqttTopicB() + "/BTtoMQTT/";
                        topic += info.address().toString().remove(':');

                        status = mq->publish(topic, QString::fromStdString(output));
                    }

                    status = true;
                }
                else
                {
                    //std::string input;
                    //serializeJson(doc, input);
                    //qDebug() << "input :" << input.c_str();
                }
            }

            // Dynamic updates
            if (m_listening)
            {
                if (!dd->hasBluetoothConnection()) return;
                if (dd->getName() == "ThermoBeacon") return;

                if (dd->needsUpdateRt())
                {
                    // old or no data: go for refresh
                    m_devices_updating_queue.push_back(dd);
                    dd->refreshQueue();
                    refreshDevices_continue();
                }
            }

            return;
        }
    }

    if (!status) // UN-KNOWN DEVICES ///////////////////////////////////////////
    {
        const QList<quint16> &manufacturerIds = info.manufacturerIds();
        for (const auto id: manufacturerIds)
        {
            //qDebug() << info.name() << info.address() << Qt::hex
            //         << "ID" << id
            //         << "manufacturer data" << Qt::dec << info.manufacturerData(id).count() << Qt::hex
            //         << "bytes:" << info.manufacturerData(id).toHex();

            DynamicJsonDocument doc(512);
            doc["id"] = info.address().toString().toStdString();
            doc["name"] = info.name().toStdString();
            doc["manufacturerdata"] = QByteArray::number(endian_flip_16(id), 16).toStdString() + info.manufacturerData(id).toHex().toStdString();
            doc["rssi"] = info.rssi();

            TheengsDecoder dec;
            JsonObject obj = doc.as<JsonObject>();

            if (dec.decodeBLEJson(obj) >= 0)
            {
                obj.remove("manufacturerdata");

                std::string output;
                serializeJson(obj, output);
                //qDebug() << "(UNKNOWN DEVICE) output (mfd) " << output.c_str();

                SettingsManager *sm = SettingsManager::getInstance();
                MqttManager *mq = MqttManager::getInstance();
                if (sm && mq)
                {
                    QString topic = sm->getMqttTopicA() + "/" + sm->getMqttTopicB() + "/BTtoMQTT/";
                    topic += info.address().toString().remove(':');

                    mq->publish(topic, QString::fromStdString(output));
                }

                status = true;
            }
        }

        const QList<QBluetoothUuid> &serviceIds = info.serviceIds();
        for (const auto id: serviceIds)
        {
            //qDebug() << info.name() << info.address() << Qt::hex
            //         << "ID" << id
            //         << "service data" << Qt::dec << info.serviceData(id).count() << Qt::hex
            //         << "bytes:" << info.serviceData(id).toHex();

            DynamicJsonDocument doc(512);
            doc["id"] = info.address().toString().toStdString();
            doc["name"] = info.name().toStdString();
            doc["servicedata"] = info.serviceData(id).toHex().toStdString();
            doc["servicedatauuid"] = id.toString(QUuid::Id128).toStdString();
            doc["rssi"] = info.rssi();

            TheengsDecoder dec;
            JsonObject obj = doc.as<JsonObject>();

            if (dec.decodeBLEJson(obj) >= 0)
            {
                obj.remove("servicedata");
                obj.remove("servicedatauuid");

                std::string output;
                serializeJson(obj, output);
                //qDebug() << "(UNKNOWN DEVICE) output (svd)" << output.c_str();

                SettingsManager *sm = SettingsManager::getInstance();
                MqttManager *mq = MqttManager::getInstance();
                if (sm && mq)
                {
                    QString topic = sm->getMqttTopicA() + "/" + sm->getMqttTopicB() + "/BTtoMQTT/";
                    topic += info.address().toString().remove(':');

                    status = mq->publish(topic, QString::fromStdString(output));
                }

                status = true;
            }
        }
    }

    // Dynamic scanning
    if (m_scanning)
    {
        //qDebug() << "addBleDevice() FROM DYNAMIC SCANNING";
        addBleDevice(info);
    }
}

/* ************************************************************************** */
