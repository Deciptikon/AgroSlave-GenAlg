#include "neuron.h"

Neuron::Neuron()
{

}

Neuron::Neuron(QList<float> wieght)
{
    if(!wieght.isEmpty()) {
        this->wieght.clear();
        this->wieght = wieght;

        this->k = 1;
        this->b = 0;
    }
}

Neuron::Neuron(int numWieght, float maxValue)
{
    if(numWieght>0 && maxValue>0) {
        this->wieght.clear();
        for(int i=0; i<numWieght; i++) {
            float w = QRandomGenerator::global()->bounded(2*maxValue);
            this->wieght.append(w - maxValue);
        }
        float r = QRandomGenerator::global()->bounded(maxValue);
        this->k = r;
        r = QRandomGenerator::global()->bounded(2*maxValue);
        this->b = r - maxValue;
    }
}

void Neuron::debug()
{
    if(this->wieght.isEmpty()) {
        qDebug() << "this->wieght.isEmpty()";
    } else {
//        for(auto w: this->wieght) {
//            qDebug() << "W =" << w;
//        }
        qDebug() << "Debug Neuron:" << this->wieght << "| k =" << k << "| b ="<< b;
    }
}

void Neuron::run(QList<float> &data)
{
    if(!data.isEmpty() && !this->wieght.isEmpty() && data.size() == this->wieght.size()) {
        float sum = 0;

//        for(int i=0; i<data.size(); i++) {
//            sum += data.at(i)*this->wieght.at(i);
//        }

        auto iD = data.begin();
        auto iW = wieght.begin();
        while (iD != data.end()) {
            sum += (*iD) * (*iW);
            ++iD;
            ++iW;
        }

        this->selfValue = 1.0f / (1.0f + expf( -sum * k + b));
    }
}

void Neuron::run(QList<Neuron> &data)
{
    if(!data.isEmpty() && !this->wieght.isEmpty() && data.size() == this->wieght.size()) {
        float sum = 0;

//        for(int i=0; i<data.size(); i++) {
//            sum += data.at(i).getSelfValue()*this->wieght.at(i);
//        }

        auto iD = data.begin();
        auto iW = wieght.begin();
        while (iD != data.end()) {
            sum += (*iD).getSelfValue() * (*iW);
            ++iD;
            ++iW;
        }

        this->selfValue = 2.0f / (1.0f + expf( -sum * k + b)) - 1.0f;
    }
}

Neuron Neuron::Crossingover(Neuron &n1, Neuron &n2)
{
    QList<float> w;

/*    int size = n1.getWieght().size();
    for(int i=0; i<size; i++) {
        float r = QRandomGenerator::global()->bounded(1.0);
        if(r>0.5) {
            w.append(n1.getWieght().at(i));
        } else {
            w.append(n2.getWieght().at(i));
        }
    }*/
    auto W1 = n1.getWieght();
    auto W2 = n2.getWieght();
    auto iW1 = W1.begin();//итератор по весам нейрона1
    auto iW2 = W2.begin();
    while(iW1 != W1.end()) {
        float r = QRandomGenerator::global()->bounded(1.0);
        if(r>0.5) {
            float v = *iW1;
            w.append(v);
        } else {
            float v = *iW2;
            w.append(v);
        }
        ++iW1;
        ++iW2;
    }
    float k;
    float b;
    if(QRandomGenerator::global()->bounded(1.0) > 0.5) {
        k = n1.getK();
    } else {
        k = n2.getK();
    }
    if(QRandomGenerator::global()->bounded(1.0) > 0.5) {
        b = n1.getB();
    } else {
        b = n2.getB();
    }

    Neuron n3(w);
    n3.setK(k);
    n3.setB(b);
    return n3;
}

Neuron Neuron::Mutation(Neuron n, float maxValueMutation)
{
    QList<float> w;
    for(auto v: n.getWieght()) {
        float r = QRandomGenerator::global()->bounded(2*maxValueMutation);
        w.append(v + r - maxValueMutation);
    }
    Neuron n2(w);
    float r = QRandomGenerator::global()->bounded(2*maxValueMutation);
    float k = n.getK() + r - maxValueMutation;
    //if(k<0) {k=0;}
    n2.setK(k);
    r = QRandomGenerator::global()->bounded(2*maxValueMutation);
    n2.setB(n.getB() + r - maxValueMutation);
    return n2;
}

float Neuron::getSelfValue() const
{
    return selfValue;
}

QList<float> Neuron::getWieght() const
{
    return wieght;
}

void Neuron::setWieght(const QList<float> &value)
{
    wieght = value;
}

float Neuron::getK() const
{
    return k;
}

void Neuron::setK(float value)
{
    if(value < 0) {
        k = 0;
    } else {
        k = value;
    }
}

float Neuron::getB() const
{
    return b;
}

void Neuron::setB(float value)
{
    b = value;
}

void Neuron::setSelfValue(float value)
{
    selfValue = value;
}
