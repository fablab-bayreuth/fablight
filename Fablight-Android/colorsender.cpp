#include "colorsender.h"

ColorSender::ColorSender(QObject *parent) :
    QObject(parent),
    socket(0),
    waiting(false),
    queued(false)
{
}

void ColorSender::setSocket(QBluetoothSocket *socket) {
    if(this->socket) {
        disconnect(this->socket, SIGNAL(readyRead()),
                   this, SLOT(readyWrite()));
    }

    this->socket = socket;
    connect(this->socket, SIGNAL(readyRead()),
            this, SLOT(readyWrite()));

    waiting = false;
    if(queued) {
        send();
    }
}

void ColorSender::send() {
    if(socket == 0) {
        return;
    }
    socket->write(queuedData.toLatin1());
    waiting = true;
    queued = false;
    qDebug() << "Sending: " + queuedData;
}

void ColorSender::sendRGBW(unsigned int fadeTime, int red, int green, int blue, int white) {
    QString data("%1%2%3%4%5\r");
    data = tr("r")
            + data.arg((int)fadeTime, 4, 16, QChar('0'))
                  .arg((int)white,    2, 16, QChar('0'))
                  .arg((int)red,      2, 16, QChar('0'))
                  .arg((int)green,    2, 16, QChar('0'))
                  .arg((int)blue,     2, 16, QChar('0'))
                  .toUpper();
    queue(data);
}

void ColorSender::queue(QString data) {
    queuedData = data;
    queued = true;
    if(!waiting) {
        send();
    }
}

void ColorSender::readyWrite() {
    qDebug() << "Confirmation";
    waiting = false;
    if(queued) {
        send();
    }
}
