#include "genalg.h"

GenAlg::GenAlg()
{

}

void GenAlg::init(int maxCount, NeuroNet &net)
{
    if(maxCount>1) {
        MAX_COUNT = maxCount;
    } else {
        MAX_COUNT = 2;
    }
    generateMutation  = 10;
    maxValueMutation  = 1;
    probalityMutation = 0.3;

    population.clear();
    if(!net.isEmpty()) {
        population.append(net);
    }
    while(population.size()< MAX_COUNT*5) {
        NeuroNet nn(net.getTopology(), generateMutation);
        population.append(nn);
    }
}

void GenAlg::update(const int &count, float angle)
{
    //qDebug() << "void GenAlg::update(const int &count, QVector3D angle)";
    if(count < 0) {
        //nihil
        qDebug() << "//nihil:    count =" << count;
    } else {
        if(count < MAX_COUNT*5) {
            //calculate adaptions

            NeuroNet nn = population.at(count);
            nn.setAdaption(angle);
            population.removeAt(count);
            population.insert(count, nn);

            //qDebug() << "//calculate adaptions:" << count << " =>" << angle;
        }
    }
}

void GenAlg::run()
{
    Selection();
    Crossingover();
    Mutation();
}

void GenAlg::Crossingover()
{
    //qDebug() << "void GenAlg::Crossingover()";
    if(!population.isEmpty()) {
        QList<NeuroNet> crossPop;
        for(NeuroNet netA: population) {
            int r = QRandomGenerator::global()->bounded(population.size());
            NeuroNet netB = population.at(r);
            NeuroNet netC = NeuroNet::Crossingover2(netA, netB);
            crossPop.append(netC);
        }
        if(!crossPop.isEmpty()) {
            population.append(crossPop);
        }
    }
}

void GenAlg::Mutation()
{
    //qDebug() << "void GenAlg::Mutation()";
    if(!population.isEmpty()) {
        QList<NeuroNet> mutPop;
        for(NeuroNet net: population) {
            NeuroNet nn = NeuroNet::Mutation(net, maxValueMutation, probalityMutation);
            mutPop.append(nn);
        }
        population.append(mutPop);

        //add random obj
        for(int i=0; i< MAX_COUNT; i++) {
            //NeuroNet nn(population.first().getTopology(), generateMutation);
            NeuroNet nn(population.first(), generateMutation);
            population.append(nn);
        }
    }
}

void GenAlg::Selection()
{
    qDebug() << "void GenAlg::Selection()";
    if(!population.isEmpty()) {
        std::sort(population.begin(), population.end(),
              [](NeuroNet a,  NeuroNet b) -> bool { return a.getAdaption() < b.getAdaption(); });
//        qSort(population.begin(), population.end(),
//              [](NeuroNet a,  NeuroNet b) -> bool { return a.getAdaption() < b.getAdaption(); });
//        for(NeuroNet nn: population) {
//            qDebug() << "getAdaption()" << nn.getAdaption();
//        }
        while(population.size() > MAX_COUNT) {
            population.removeLast();
        }
        qDebug() << "Лучшая особь =" << population.first().getAdaption();
    }
}

QList<NeuroNet> GenAlg::getPopulation() const
{
    return population;
}

void GenAlg::setPopulation(const QList<NeuroNet> &value)
{
    population = value;
}

void GenAlg::setGenerateMutation(float value)
{
    generateMutation = value;
}

void GenAlg::setProbalityMutation(float value)
{
    probalityMutation = value;
}

void GenAlg::setMaxValueMutation(float value)
{
    if(value>0) {
        maxValueMutation = value;
    } else {
        maxValueMutation = 0;
    }

}
