/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "slidersgroup.h"
#include "window.h"

//! [0]
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
