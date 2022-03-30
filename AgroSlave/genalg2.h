#ifndef GENALG2_H
#define GENALG2_H

#include <QObject>
#include <QDebug>
#include "neuronet.h"

typedef QList<float> ListFloat;

class GenAlg2 : public QObject
{
    Q_OBJECT
public:
    GenAlg2(QObject *parent = nullptr);

    void init(NeuroNet net, size_t sizePop);

    void setMaxIter(const size_t &value);



    void setInputOutputData(const ListFloat &input, const ListFloat &output);

signals:
    void toRun();
    void restartThread();
    void sendNeuroNet(const NeuroNet net);

    void currentError(const float &err);
    void updateEpoha(const int& num);

public slots:
    void run();

    void setRunningTrue();
    void setRunningFalse();
    void swapRunning();

    void setGenerateMutation(float value);

    void setMaxValueMutation(float value);

    void setProbalityMutation(float value);

private slots:
    //

private:
    void mutation();
    void crossingover();
    void adaption();
    void selection();
    QList<NeuroNet> createRndObj( NeuroNet net, size_t sizePop);

    bool isRunning;

    size_t SIZE_POPULATION;
    size_t MAX_COUNT_RND_OBJ;
    size_t MAX_ITER;
    size_t iter;

    float generateMutation;//generate first population
    float maxValueMutation;//maximum mutation
    float probalityMutation;//


    QList<NeuroNet> population;

    ListFloat inputData;
    ListFloat outputData;
};

#endif // GENALG2_H
