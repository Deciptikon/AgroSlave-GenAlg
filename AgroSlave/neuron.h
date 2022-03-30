#ifndef NEURON_H
#define NEURON_H

#include <QList>
#include <QRandomGenerator>
#include <QDebug>

class Neuron
{
public:
    Neuron();
    Neuron(QList<float> wieght);
    Neuron(int numWieght, float maxValue);

    void debug();

    void run(QList<float> &data);
    void run(QList<Neuron> &data);

    static Neuron Crossingover(Neuron &n1, Neuron &n2);
    static Neuron Mutation(Neuron n, float maxValueMutation);

    float getSelfValue() const;
    void setSelfValue(float value);

    QList<float> getWieght() const;
    void setWieght(const QList<float> &value);



    float getK() const;
    void setK(float value);

    float getB() const;
    void setB(float value);

private:
    QList<float> wieght;
    float selfValue;

    float k;
    float b;
};

#endif // NEURON_H
