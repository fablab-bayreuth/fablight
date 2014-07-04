#ifndef COLORSENDER_H
#define COLORSENDER_H

#include <QObject>
#include <QtBluetooth/QBluetoothSocket>

class ColorSender : public QObject
{
    Q_OBJECT
public:
    explicit ColorSender(QObject *parent = 0);

signals:

public slots:
    void sendRGBW(unsigned int fadeTime, int red, int green, int blue, int white);
    //void sendLab(unsigned int fadeTime, int L, int a, int b);
    void setSocket(QBluetoothSocket *socket);

private:
    enum ColorType { RGBW, Lab };

    QBluetoothSocket *socket;

    bool waiting;
    bool queued;
    QString queuedData;

    void queue(QString data);

private slots:
    void send();
    void readyWrite();
};

#endif // COLORSENDER_H
