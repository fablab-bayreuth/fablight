#include <QtWidgets>

#include "slidersgroup.h"

SlidersGroup::SlidersGroup(Qt::Orientation orientation, const QString &title,
                           QWidget *parent)
    : QWidget(parent)
{
    name = new QLabel(title);

    value = new QSpinBox();
    value->setMaximum(255);
    value->setMinimum(0);

    slider = new QSlider(orientation);
    slider->setMaximum(255);
    slider->setMinimum(0);
    slider->setFocusPolicy(Qt::StrongFocus);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(16);
    slider->setSingleStep(1);

    connect(slider, SIGNAL(valueChanged(int)),
            value, SLOT(setValue(int)));
    connect(value, SIGNAL(valueChanged(int)),
            slider, SLOT(setValue(int)));
    connect(slider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
    connect(value, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));

    QBoxLayout::Direction direction;

    if (orientation == Qt::Horizontal)
        direction = QBoxLayout::LeftToRight;
    else
        direction = QBoxLayout::TopToBottom;

    QBoxLayout *slidersLayout = new QBoxLayout(direction);
    slidersLayout->addWidget(name);
    slidersLayout->addWidget(value);
    slidersLayout->addWidget(slider);
    setLayout(slidersLayout);
}

void SlidersGroup::setValue(int v)
{
    slider->setValue(v);
    value->setValue(v);
}

void SlidersGroup::setMinimum(int v)
{
    slider->setMinimum(v);
    value->setMinimum(v);
}

void SlidersGroup::setMaximum(int v)
{
    slider->setMaximum(v);
    value->setMaximum(v);
}


int SlidersGroup::getValue() {
    return slider->value();
}
