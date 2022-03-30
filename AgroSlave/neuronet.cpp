#include "neuronet.h"

NeuroNet::NeuroNet()
{

}

NeuroNet::NeuroNet(QList<QList<Neuron> > net)
{
    if(!net.isEmpty()) {
        this->net = net;
        this->topology.clear();
        for(auto s: net) {
            this->topology.append(s.size());
        }
    }
}

NeuroNet::NeuroNet(QList<int> topology, float maxValue)
{
    if(!topology.isEmpty()) {
        int b=0;
        this->topology = topology;
        this->net.clear();
        for(int s: topology) {
            QList<Neuron> layer;

            for(int i=0; i<s; i++) {
                if(b==0) {
                    QList<float> w;
                    w.append(1);
                    Neuron n(w);
                    layer.append(n);
                } else {
                    Neuron n(b, maxValue);
                    layer.append(n);
                }
            }
            b=s;
            this->net.append(layer);
        }
    }
}

NeuroNet::NeuroNet(NeuroNet &net, float maxValue)
{

    if(!net.isEmpty()) {
        NeuroNet nn = NeuroNet::Mutation(net, maxValue, 1);
        this->net = nn.getNet();
        this->topology = nn.getTopology();
    } else {
        qDebug() << "NeuroNet::NeuroNet(NeuroNet &net, float maxValue)::net.isEmpty !!!";
    }
}

void NeuroNet::run(QList<float> &data)
{
    if(!data.isEmpty() && !this->net.isEmpty() && data.size()==this->net.first().size()) {
        QList< QList<Neuron> > nn;
        QList<Neuron> layerA, layerB;
/*        for(int i=0; i<data.size(); i++) {
            Neuron n = net.first().at(i);
            n.setSelfValue(data.at(i));
            //qDebug() << "self value" << n.getSelfValue();
            layerA.append(n);
        }*/

        //в нейроны первого слоя данные записываются как собственные значения
        //и будут прочитаны и обработаны последуюими слоями
        auto iD = data.begin();//итератор по данным
        auto iN = net.first().begin();//итератор по нейронам первого слоя
        while(iD != data.end()) {
            Neuron n = *iN;
            n.setSelfValue(*iD);
            layerA.append(n);
            ++iD;
            ++iN;
        }
        nn.append(layerA);

/*        if(this->net.size() > 1) {
            for(int i=1; i < this->net.size(); i++) {
                layerB.clear();
                layerB = layerA;
                layerA.clear();

                for(int j=0; j<this->net.at(i).size(); j++) {
                    Neuron n = net.at(i).at(j);
                    n.run(layerB);
                    //qDebug() << "self value" << n.getSelfValue();
                    layerA.append(n);
                }
                nn.append(layerA);
            }
        }*/

        if(this->net.size() > 1) {
            auto iL = net.begin();//итератор по слоям
            ++iL;//первый слой обработан ранее
            while(iL != net.end()) {
                layerB.clear();
                layerB = layerA;
                layerA.clear();

                auto iN = iL->begin();
                while (iN != iL->end()) {
                    Neuron n = *iN;
                    n.run(layerB);
                    layerA.append(n);
                    ++iN;
                }
                nn.append(layerA);
                ++iL;
            }

        }

        this->net.clear();
        this->net = nn;

        this->selfValues.clear();
        for(Neuron n: this->net.last()) {
            this->selfValues.append(n.getSelfValue());
        }
    }
}

NeuroNet NeuroNet::Crossingover(NeuroNet &nn1, NeuroNet &nn2)
{
    //qDebug() << "NeuroNet NeuroNet::Crossingover(NeuroNet &nn1, NeuroNet &nn2) --> START";
    NeuroNet nn;
/*    if(!nn1.isEmpty() && !nn2.isEmpty()) {
        QList< QList<Neuron> > net;
        for(int i=0; i<nn1.getNet().size(); i++) {
            QList<Neuron> layer;
            for(int j=0; j<nn1.getNet().at(i).size(); j++) {
                Neuron n;
                float rnd = QRandomGenerator::global()->bounded(1.0);
                //qDebug() << "rnd =" << rnd;
                if(rnd<0.5) {
                    n = nn1.getNet().at(i).at(j);
                } else {
                    n = nn2.getNet().at(i).at(j);
                }
                layer.append(n);
            }
            net.append(layer);
        }
        nn.setNet(net);
    }*/
    if(!nn1.isEmpty() && !nn2.isEmpty()) {
        //qDebug() << "---> 1";
        QList< QList<Neuron> > net;
        auto net1 = nn1.getNet();
        auto net2 = nn2.getNet();
        auto iL1 = net1.begin();//итератор по слоям сети
        auto iL2 = net2.begin();
        //qDebug() << "---> 2";
        while(iL1 != net1.end()) {
            QList<Neuron> layer;
            //qDebug() << "---> 3";
            auto iN1 = iL1->begin();//итератор по нейронам слоя
            auto iN2 = iL2->begin();
            //qDebug() << "---> 4";
            while(iN1 != iL1->end()) {
                //qDebug() << "---> 5";
                Neuron n;
                float rnd = QRandomGenerator::global()->bounded(1.0);
                if(rnd<0.5) {
                    n = *iN1;
                } else {
                    n = *iN2;
                }
                layer.append(n);
                ++iN1;
                ++iN2;
                //qDebug() << "---> 6";
            }
            net.append(layer);
            ++iL1;
            ++iL2;
            //qDebug() << "---> 7";
        }
        nn.setNet(net);

    }
    //qDebug() << "NeuroNet NeuroNet::Crossingover(NeuroNet &nn1, NeuroNet &nn2) --> END";
    return nn;
}

NeuroNet NeuroNet::Crossingover2(NeuroNet &nn1, NeuroNet &nn2)
{
    //qDebug() << "NeuroNet NeuroNet::Crossingover2(NeuroNet &nn1, NeuroNet &nn2) --> START";
    NeuroNet nn;
    if(!nn1.isEmpty() && !nn2.isEmpty()) {
        //qDebug() << "---> 1";
        QList< QList<Neuron> > net;
        auto net1 = nn1.getNet();
        auto net2 = nn2.getNet();
        auto iL1 = net1.begin();//итератор по слоям сети
        auto iL2 = net2.begin();
        //qDebug() << "---> 2";
        while(iL1 != net1.end()) {
            QList<Neuron> layer;
            //qDebug() << "---> 3";
            auto iN1 = iL1->begin();//итератор по нейронам слоя
            auto iN2 = iL2->begin();
            //qDebug() << "---> 4";
            while(iN1 != iL1->end()) {
                //qDebug() << "---> 5";
                Neuron n = Neuron::Crossingover( *iN1, *iN2);
                layer.append(n);
                ++iN1;
                ++iN2;
                //qDebug() << "---> 6";
            }
            net.append(layer);
            ++iL1;
            ++iL2;
            //qDebug() << "---> 7";
        }
        nn.setNet(net);

    }
    //qDebug() << "NeuroNet NeuroNet::Crossingover2(NeuroNet &nn1, NeuroNet &nn2) --> END";
    return nn;
}

NeuroNet NeuroNet::Mutation(NeuroNet &nn, float maxValueMutation, float probalityMutation)
{
    NeuroNet nnMut;
    if(!nn.isEmpty()) {
        QList< QList<Neuron> > netMut;
        for(auto layer: nn.getNet()) {
            QList<Neuron> layerMut;
            for(Neuron n: layer) {
                Neuron nMut;
                float rnd = QRandomGenerator::global()->bounded(1.0);
                if(rnd<probalityMutation) {
                    nMut = Neuron::Mutation(n, maxValueMutation);
                } else {
                    nMut = n;
                }
                layerMut.append(nMut);
            }
            netMut.append(layerMut);
        }
        nnMut.setNet(netMut);
    }
    return nnMut;
}

void NeuroNet::debugNet()
{
    qDebug() << "---------------Debug Neuro Net:----------------";
    if(!this->net.isEmpty()) {
        for(int i=0; i<this->net.size(); i++) {
            qDebug() << "Layer" << i;
            for(Neuron n: this->net.at(i)) {
                n.debug();
            }
        }
    } else {
        qDebug() << "Neuro Net is Empty";
    }
}

void NeuroNet::debugTopology()
{
    qDebug() << "---------------Debug Topology Neuro Net:----------------";
    if(!this->topology.isEmpty()) {
        qDebug() << this->topology;
    } else {
        qDebug() << "Topology is Empty";
    }
}

QList<float> NeuroNet::getSelfValues() const
{
    return selfValues;
}

bool NeuroNet::isEmpty()
{
    return net.isEmpty();
}

QList<QList<Neuron> > NeuroNet::getNet() const
{
    return net;
}

void NeuroNet::setNet(const QList< QList<Neuron> > &net)
{
    if(!net.isEmpty()) {
        this->net.clear();
        this->net = net;
        this->topology.clear();
        for(auto s: net) {
            this->topology.append(s.size());
        }
    }
}

QList<int> NeuroNet::getTopology() const
{
    return topology;
}

float NeuroNet::getAdaption() const
{
    return adaption;
}

void NeuroNet::setAdaption(float value)
{
    adaption = value;
}
