#ifndef NEURONET_H
#define NEURONET_H

#include "neuron.h"

class NeuroNet
{
public:
    NeuroNet();
    NeuroNet(QList< QList<Neuron> > net);
    NeuroNet(QList<int> topology, float maxValue);
    NeuroNet(NeuroNet &net, float maxValue);

    void run(QList<float> &data);

    //скрещевает на уровне нейронов
    static NeuroNet Crossingover(NeuroNet &nn1, NeuroNet &nn2);

    //скрещевает на уровне весов
    static NeuroNet Crossingover2(NeuroNet &nn1, NeuroNet &nn2);

    static NeuroNet Mutation(NeuroNet &nn, float maxValueMutation, float probalityMutation);

    void debugNet();
    void debugTopology();

    QList<float> getSelfValues() const;

    bool isEmpty();

    QList<QList<Neuron> > getNet() const;
    void setNet(const QList<QList<Neuron> > &net);

    QList<int> getTopology() const;

    float getAdaption() const;
    void setAdaption(float value);

private:
    QList<int> topology;
    QList< QList<Neuron> > net;
    QList<float> selfValues;
    float adaption;
};

#endif // NEURONET_H
