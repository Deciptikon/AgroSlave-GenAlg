#include "mainwindow.h"

#include <QApplication>

#include <QList>
#include "neuronet.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QList<float> d1;
//    for(int i=0; i<6; i++) {
//        float w = QRandomGenerator::global()->bounded(1);
//        d1.append(w);
//    }

//    Neuron n1(6, 10.7);
//    Neuron n2(6, 1.7);
//    Neuron n3 = Neuron::Mutation(n1,1);
//    Neuron n4 = Neuron::Mutation(n2,1);
//    n1.debug();
//    n2.debug();
//    n3.debug();
//    n4.debug();

//    QList<float> d{0.2, 0.3, 0.4};

//    QList<int> t1{3,4,5};
//    NeuroNet nn1(t1, 1);
//    nn1.debugTopology();
//    nn1.debugNet();

//    QList<int> t2{3,4,5};
//    NeuroNet nn2(t2, 1);
//    nn2.debugTopology();
//    nn2.debugNet();

//    NeuroNet nn3 = NeuroNet::Crossingover(nn1,nn2);
//    nn3.debugTopology();
//    nn3.debugNet();
//    nn3.run(d);
//    qDebug() << "RUN:" << nn1.getSelfValues();





//    QList<float> d{0.86};

//    NeuroNet net1({1,2,3,1}, 10);

//    NeuroNet net2({1,3,2,1}, 10);
//    NeuroNet net3({1,2,2,1}, 10);
//    net1.run(d);
//    qDebug() << net1.getSelfValues();
//    net2.run(d);
//    qDebug() << net2.getSelfValues();
//    net3.run(d);
//    qDebug() << net3.getSelfValues();

    MainWindow w;
    w.show();
    return a.exec();
}
