#ifndef FILTRKALMANA_H
#define FILTRKALMANA_H

#include <QObject>
#include <QDebug>
#include <eigen3/Eigen/Dense>//библиотека матриц

using Eigen::MatrixXd;

class FiltrKalmana : public QObject
{
    Q_OBJECT
public:
    explicit FiltrKalmana(QObject *parent = nullptr);

    void init(double dt, int n, int m);


    Eigen::MatrixXd getX_hat() const;

    void setX_hat(const Eigen::MatrixXd &value);

signals:

public slots:
    void update(Eigen::MatrixXd y);

private:

    double dt;

    int n; // Number of states
    int m; // Number of measurements

    Eigen::MatrixXd A; // System dynamics matrix
    Eigen::MatrixXd C; // Output matrix
    Eigen::MatrixXd Q; // Process noise covariance
    Eigen::MatrixXd R; // Measurement noise covariance
    Eigen::MatrixXd P; // Estimate error covariance
    Eigen::MatrixXd I; // Identity matrix
    Eigen::MatrixXd x_hat;
};

#endif // FILTRKALMANA_H
