#include <QtWidgets>

#include "slidersgroup.h"
#include "window.h"

Window::Window()
{
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
            &sender, SLOT(setSocket(QBluetoothSocket*)));
}

void Window::sendChanges() {
    sender.sendRGBW(0, red->getValue(), green->getValue(), blue->getValue(), white->getValue());
}
