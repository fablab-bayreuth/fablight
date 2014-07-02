#include "bluetoothselector.h"

BluetoothSelector::BluetoothSelector(QWidget *parent) :
    QWidget(parent),
    list(),
    button(tr("Scanning..."))
{
    QBluetoothLocalDevice localDevice;
    QBluetoothAddress adapterAddress = localDevice.address();

    localDevice.powerOn();

    discoveryAgent = new QBluetoothServiceDiscoveryAgent(this);

    discoveryAgent->setUuidFilter(QBluetoothUuid(tr("00001101-0000-1000-8000-00805f9b34fb")));

    connect(discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
            this, SLOT(addService(QBluetoothServiceInfo)));
    connect(discoveryAgent, SIGNAL(finished()),
            this, SLOT(finishedScanning()));
    connect(&button, SIGNAL(clicked()),
            this, SLOT(buttonClicked()));
    connect(&socket, SIGNAL(connected()),
            this, SLOT(emitConnected()));
    connect(&socket, SIGNAL(disconnected()),
            this, SIGNAL(disconnected()));

    discoveryAgent->start();

    button.setDisabled(true);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(&list);
    layout->addWidget(&button);
    this->setLayout(layout);
}

void BluetoothSelector::addService(QBluetoothServiceInfo info) {
    QString line = info.device().name();
    line.append(tr(": ") + info.device().address().toString());

    list.addItem(line);
    hash[line] = info;
}

void BluetoothSelector::finishedScanning() {
    button.setEnabled(true);
    button.setText("Connect");
}

void BluetoothSelector::buttonClicked() {
    switch(socket.state()) {
    case QBluetoothSocket::UnconnectedState:
        connectToService();
        break;

    default:
        disconnectFromService();
        break;
    }
}

void BluetoothSelector::disconnectFromService() {
    socket.disconnectFromService();
    button.setText(tr("Connect"));
}

void BluetoothSelector::connectToService() {
    button.setText("Connecting...");
    socket.connectToService(hash[list.currentText()]);
    button.setText("Disconnect");
}

void BluetoothSelector::emitConnected() {
    emit connected(&socket);
}
