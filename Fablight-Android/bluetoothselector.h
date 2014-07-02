#ifndef BLUETOOTHSELECTOR_H
#define BLUETOOTHSELECTOR_H

#include <QHBoxLayout>
#include <QPushButton>
#include <QtBluetooth/QtBluetooth>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QBluetoothServiceDiscoveryAgent>
#include <QtBluetooth/QBluetoothSocket>
#include <QWidget>
#include <QComboBox>
#include <QHash>

class BluetoothSelector : public QWidget
{
    Q_OBJECT
public:
    explicit BluetoothSelector(QWidget *parent = 0);

signals:
    void connected(QBluetoothSocket *socket);
    void disconnected();

public slots:
    void connectToService();
    void disconnectFromService();

private:
    QBluetoothServiceDiscoveryAgent *discoveryAgent;
    QComboBox list;
    QPushButton button;

    QBluetoothSocket socket;

    QHash<QString, QBluetoothServiceInfo> hash;

private slots:
    void addService(QBluetoothServiceInfo info);
    void finishedScanning();
    void buttonClicked();
    void emitConnected();

};

#endif // BLUETOOTHSELECTOR_H
