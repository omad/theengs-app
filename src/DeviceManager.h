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

#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H
/* ************************************************************************** */

#include "DeviceFilter.h"

#include <QObject>
#include <QVariant>
#include <QList>
#include <QTimer>

#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>
QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceInfo)
QT_FORWARD_DECLARE_CLASS(QLowEnergyController)
QT_FORWARD_DECLARE_CLASS(QLowEnergyConnectionParameters)

/* ************************************************************************** */

//#define DEBUG_FAKE_DEVICES // Uncomment to enable virtual devices // AND REBUILD

/* ************************************************************************** */

/*!
 * \brief The DeviceManager class
 */
class DeviceManager: public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool hasDevices READ areDevicesAvailable NOTIFY devicesListUpdated)
    Q_PROPERTY(DeviceFilter *devicesList READ getDevicesFiltered NOTIFY devicesListUpdated)
    Q_PROPERTY(int deviceCount READ getDeviceCount NOTIFY devicesListUpdated)

    Q_PROPERTY(bool listening READ isListening NOTIFY listeningChanged)
    Q_PROPERTY(bool scanning READ isScanning NOTIFY scanningChanged)
    Q_PROPERTY(bool updating READ isUpdating NOTIFY updatingChanged)
    Q_PROPERTY(bool syncing READ isSyncing NOTIFY syncingChanged)
    Q_PROPERTY(bool advertising READ isAdvertising NOTIFY advertisingChanged)

    Q_PROPERTY(bool bluetooth READ hasBluetooth NOTIFY bluetoothChanged)
    Q_PROPERTY(bool bluetoothAdapter READ hasBluetoothAdapter NOTIFY bluetoothChanged)
    Q_PROPERTY(bool bluetoothEnabled READ hasBluetoothEnabled NOTIFY bluetoothChanged)
    Q_PROPERTY(bool bluetoothPermissions READ hasBluetoothPermissions NOTIFY bluetoothChanged)

    Q_PROPERTY(DeviceFilter *devicesNearby READ getDevicesNearby NOTIFY devicesNearbyUpdated)

    static const int ble_scanning_duration = 60;
    static const int ble_listening_duration = 0;
    static const int ble_listening_duration_nearby = 0;
    static const int ble_listening_duration_background = 60;

    bool m_dbInternal = false;
    bool m_dbExternal = false;

    bool m_daemonMode = false;

    bool m_btA = false;
    bool m_btE = false;
    bool m_btP = true;

    QBluetoothLocalDevice *m_bluetoothAdapter = nullptr;
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent = nullptr;
    QLowEnergyConnectionParameters *m_ble_params = nullptr;

    QList <QString> m_devices_blacklist;

    DeviceModel *m_devices_nearby_model = nullptr;
    DeviceFilter *m_devices_nearby_filter = nullptr;

    DeviceModel *m_devices_model = nullptr;
    DeviceFilter *m_devices_filter = nullptr;

    QList <QObject *> m_devices_updating_queue;
    QList <QObject *> m_devices_updating;

    QList <QObject *> m_devices_syncing_queue;
    QList <QObject *> m_devices_syncing;

    bool m_listening = false;
    bool isListening() const;

    bool m_scanning = false;
    bool isScanning() const;

    bool m_updating = false;
    bool isUpdating() const;

    bool m_syncing = false;
    bool isSyncing() const;

    bool m_advertising = false;
    bool isAdvertising() const;

    bool hasBluetooth() const;
    bool hasBluetoothAdapter() const;
    bool hasBluetoothEnabled() const;
    bool hasBluetoothPermissions() const;

    void checkBluetoothIos();
    void startBleAgent();

    void setLastRun();

    // THEENGS
    QTimer m_faker;
    Device *createTheengsDevice_fromDb(const QString &deviceName, QString &deviceModel_theengs, const QString &deviceAddr);
    Device *createTheengsDevice_fromAdv(const QBluetoothDeviceInfo &deviceInfo);

    // THEENGS dev
    void fakeTheengsDevices();
    void fakeTheengsData();

Q_SIGNALS:
    void bluetoothChanged();

    void devicesListUpdated();
    void devicesNearbyUpdated();
    void devicesBlacklistUpdated();

    void listeningChanged();
    void scanningChanged();
    void updatingChanged();
    void syncingChanged();
    void advertisingChanged();

private slots:
    // QBluetoothLocalDevice related
    void bluetoothHostModeStateChanged(QBluetoothLocalDevice::HostMode);
    void bluetoothStatusChanged();

    // QBluetoothDeviceDiscoveryAgent related
    void bluetoothHostModeStateChangedIos();
    void addNearbyBleDevice(const QBluetoothDeviceInfo &info);
    void updateNearbyBleDevice(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields);
    void addBleDevice(const QBluetoothDeviceInfo &info);
    void updateBleDevice(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields);
    void updateBleDevice_simple(const QBluetoothDeviceInfo &info);
    void deviceDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error);
    void deviceDiscoveryFinished();
    void deviceDiscoveryStopped();

public:
    DeviceManager(bool daemon = false);
    ~DeviceManager();

    // Bluetooth management
    Q_INVOKABLE bool checkBluetooth();
    Q_INVOKABLE bool checkBluetoothPermissions();
    Q_INVOKABLE void enableBluetooth(bool enforceUserPermissionCheck = false);

    // Scanning management
    static int getLastRun();

    Q_INVOKABLE void scanDevices_start();
    Q_INVOKABLE void scanDevices_stop();

    Q_INVOKABLE void listenDevices_start();
    Q_INVOKABLE void listenDevices_stop();

    Q_INVOKABLE void scanNearby_start();
    Q_INVOKABLE void scanNearby_stop();

    Q_INVOKABLE void blacklistBleDevice(const QString &addr);
    Q_INVOKABLE void whitelistBleDevice(const QString &addr);
    Q_INVOKABLE bool isBleDeviceBlacklisted(const QString &addr);

    Q_INVOKABLE void disconnectDevices();

    // Device management
    Q_INVOKABLE void updateDevice(const QString &address);
    Q_INVOKABLE void syncDevice(const QString &address);
    Q_INVOKABLE void removeDevice(const QString &address);
    Q_INVOKABLE void removeDeviceData(const QString &address);

    Q_INVOKABLE void refreshDevices_background(); //!< Refresh devices on the background
    Q_INVOKABLE void refreshDevices_listen();   //!< Refresh devices with data >xh old (as they appear nearby)
    Q_INVOKABLE void refreshDevices_check();    //!< Refresh devices with data >xh old (if necessary)
    Q_INVOKABLE void refreshDevices_start();    //!< Refresh every device
    void refreshDevices_continue();
    void refreshDevices_finished(Device *dev);
    Q_INVOKABLE void refreshDevices_stop();

    Q_INVOKABLE void syncDevices_check();       //!< Sync history for devices (if necessary)
    Q_INVOKABLE void syncDevices_start();       //!< Sync history for every device
    void syncDevices_continue();
    void syncDevices_finished(Device *dev);
    Q_INVOKABLE void syncDevices_stop();

    // Devices list management
    Q_INVOKABLE bool areDevicesAvailable() const { return m_devices_model->hasDevices(); }
    DeviceFilter *getDevicesNearby() const { return m_devices_nearby_filter; }
    DeviceFilter *getDevicesFiltered() const { return m_devices_filter; }
    int getDeviceCount() const { return m_devices_model->getDeviceCount(); }

    Q_INVOKABLE void orderby_manual();
    Q_INVOKABLE void orderby_model();
    Q_INVOKABLE void orderby_name();
    Q_INVOKABLE void orderby_location();
    Q_INVOKABLE void orderby_waterlevel();
    Q_INVOKABLE void orderby_plant();
    Q_INVOKABLE void orderby_insideoutside();
    void orderby(int role, Qt::SortOrder order);

    Q_INVOKABLE QVariant getDeviceByProxyIndex(const int index) const
    {
        QModelIndex proxyIndex = m_devices_filter->index(index, 0);
        return QVariant::fromValue(m_devices_filter->data(proxyIndex, DeviceModel::PointerRole));
    }

    void invalidate();

    // THEENGS /////////////////////////////////////////////////////////////////

public:
    static Q_INVOKABLE QString getDeviceBrandTheengs(const QString &modelid);
    static Q_INVOKABLE QString getDeviceModelTheengs(const QString &modelid);
    static Q_INVOKABLE QString getDeviceTagTheengs(const QString &modelid);
    static Q_INVOKABLE QString getDevicePropsTheengs(const QString &modelid);
    QString getDeviceModelIdTheengs_fromAdv(const QBluetoothDeviceInfo &deviceInfo);

public slots:
    void discoverTheengsDevices();
};

/* ************************************************************************** */
#endif // DEVICE_MANAGER_H
