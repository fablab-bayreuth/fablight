#include <QtWidgets>

#include "slidersgroup.h"
#include "window.h"

Window::Window()
{
    socket = 0;
    white = new SlidersGroup(Qt::Horizontal, tr("White"));
    red   = new SlidersGroup(Qt::Horizontal, tr("Red"));
    green = new SlidersGroup(Qt::Horizontal, tr("Green"));
    blue  = new SlidersGroup(Qt::Horizontal, tr("Blue"));

    BluetoothSelector *s = new BluetoothSelector();
    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(s);
    layout->addWidget(white);
    layout->addWidget(red);
    layout->addWidget(green);
    layout->addWidget(blue);
    setLayout(layout);

    setWindowTitle(tr("Sliders"));

    connect(white, SIGNAL(valueChanged(int)), this, SLOT(sendChanges()));
    connect(red,   SIGNAL(valueChanged(int)), this, SLOT(sendChanges()));
    connect(green, SIGNAL(valueChanged(int)), this, SLOT(sendChanges()));
    connect(blue,  SIGNAL(valueChanged(int)), this, SLOT(sendChanges()));
    connect(s, SIGNAL(connected(QBluetoothSocket*)),
            this, SLOT(setSocket(QBluetoothSocket*)));
}

void Window::setSocket(QBluetoothSocket *socket) {
    qDebug("Connected.");
    this->socket = socket;
    stream.setDevice(socket);
}

void Window::sendChanges() {
    if((socket != 0) && (socket->state() == QBluetoothSocket::ConnectedState)) {
        stream << QString("%1,%2,%3,%4\n")
                  .arg(white->getValue())
                  .arg(blue->getValue())
                  .arg(green->getValue())
                  .arg(red->getValue());
    } else if(socket != 0) {
        qDebug("Not connected");
    }
}
