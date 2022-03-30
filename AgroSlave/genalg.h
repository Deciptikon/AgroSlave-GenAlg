#ifndef GENALG_H
#define GENALG_H

#include <QVector3D>
#include <QDebug>

#include "neuronet.h"

class GenAlg
{
public:
    GenAlg();

    void init(int maxCount, NeuroNet &net);
    void update(const int &count, float angle);
    void run();

    void setMaxValueMutation(float value);

    void setProbalityMutation(float value);

    void setGenerateMutation(float value);

    QList<NeuroNet> getPopulation() const;
    void setPopulation(const QList<NeuroNet> &value);

private:
    void Crossingover();
    void Mutation();
    //void FitFunction();
    void Selection();

    int MAX_COUNT;

    QList<NeuroNet> population;

    float generateMutation;//generate first population

    float maxValueMutation;//maximum mutation
    float probalityMutation;//
};

#endif // GENALG_H
