#include "genalg2.h"

#include <QThread>

GenAlg2::GenAlg2(QObject *parent) : QObject(parent)
{
    isRunning = false;

    SIZE_POPULATION   = 5;
    MAX_COUNT_RND_OBJ = 4;
    MAX_ITER          = 1000;
    iter              = 0;

    generateMutation  = 10;
    maxValueMutation  = 0.1;
    probalityMutation = 0.1;

    connect(this, SIGNAL(toRun()), this, SLOT(run()));
}

void GenAlg2::init(NeuroNet net, size_t sizePop)
{
    SIZE_POPULATION = sizePop;
    population.clear();
    population.append(net);
    population.append(createRndObj(net, SIZE_POPULATION - 1));
}

void GenAlg2::run()
{
    if(isRunning) {
        qDebug() << "void GenAlg2::run() is running";
        //
        population.append(createRndObj(population.first(), MAX_COUNT_RND_OBJ));
        crossingover();
        mutation();
        adaption();
        selection();


        this->thread()->msleep(100);


        qDebug() << "----------------------";
        int g=2;
        if(iter%g==0) {
            emit sendNeuroNet(population.first());
            emit currentError(population.first().getAdaption());
            emit updateEpoha(g);
        }

        iter++;
        if(iter>MAX_ITER) {
            swapRunning();
            emit sendNeuroNet(population.first());
            emit restartThread();//перезапуск потока

        }
        emit toRun();
    }
}

void GenAlg2::setRunningTrue()
{
    if(!isRunning) isRunning = true;
}

void GenAlg2::setRunningFalse()
{
    if(isRunning) isRunning = false;
}

void GenAlg2::swapRunning()
{
    isRunning = !isRunning;
}

void GenAlg2::setProbalityMutation(float value)
{
    probalityMutation = value;
}

void GenAlg2::mutation()
{
    //qDebug() << "void GenAlg::Mutation()";
    if(!population.isEmpty()) {
        QList<NeuroNet> mutPop;
        for(NeuroNet net: population) {
            NeuroNet nn = NeuroNet::Mutation(net, maxValueMutation, probalityMutation);
            mutPop.append(nn);
        }
        population.append(mutPop);


    }
}

void GenAlg2::crossingover()
{
    //qDebug() << "void GenAlg::Crossingover()";
    if(!population.isEmpty()) {
        QList<NeuroNet> crossPop;
        for(NeuroNet netA: population) {
            int r = QRandomGenerator::global()->bounded(population.size());
            NeuroNet netB = population.at(r);
            NeuroNet netC = NeuroNet::Crossingover2(netA, netB);//Crossingover(netA, netB)
            crossPop.append(netC);
        }
        if(!crossPop.isEmpty()) {
            population.append(crossPop);
        }
    }
}

void GenAlg2::adaption()
{
    //qDebug() << "void GenAlg::Adaption()";
    if(!population.isEmpty() && !inputData.isEmpty() && !outputData.isEmpty()) {
        QList<NeuroNet> adaptPop;
        for(auto net: population) {
            NeuroNet n = net;
            float summa = 0;

            auto iin = inputData.begin();
            auto iout = outputData.begin();

            while(iin != inputData.end()) {

                QList<float> in = {*iin};
                float out = *iout;

                n.run(in);

                summa += powf(n.getSelfValues().first() - out, 2);//сумма квадратов отклонений

                ++iin;
                ++iout;
            }
            n.setAdaption(summa);
            adaptPop.append(n);
        }
        population.clear();
        population.append(adaptPop);
    }
}

void GenAlg2::selection()
{
    //qDebug() << "void GenAlg::Selection()";
    if(!population.isEmpty()) {
        std::sort(population.begin(), population.end(),
              [](NeuroNet a,  NeuroNet b) -> bool { return a.getAdaption() < b.getAdaption(); });
        while(population.size() > SIZE_POPULATION) {
            population.removeLast();
        }
        qDebug() << "Лучшая особь =" << population.first().getAdaption();
    }
}

QList<NeuroNet> GenAlg2::createRndObj( NeuroNet net, size_t sizePop)
{
    QList<NeuroNet> rnd;
    //add random obj
    for(int i=0; i< sizePop; i++) {
        //NeuroNet nn(population.first().getTopology(), generateMutation);
        NeuroNet nn(net, generateMutation);
        rnd.append(nn);
    }

    return rnd;
}

void GenAlg2::setInputOutputData(const ListFloat &input, const ListFloat &output)
{
    inputData = input;
    outputData = output;
}

void GenAlg2::setMaxValueMutation(float value)
{
    maxValueMutation = value;
}

void GenAlg2::setGenerateMutation(float value)
{
    generateMutation = value;
}

void GenAlg2::setMaxIter(const size_t &value)
{
    MAX_ITER = value;
}
