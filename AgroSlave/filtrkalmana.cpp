#include "filtrkalmana.h"

FiltrKalmana::FiltrKalmana(QObject *parent) : QObject(parent)
{

}

void FiltrKalmana::init(double dt, int n, int m)
{
    this->dt = dt;
    this->n = n;
    this->m = m;

    Eigen::MatrixXd A(n, n); // System dynamics matrix
    Eigen::MatrixXd C(m, n); // Output matrix
    Eigen::MatrixXd Q(n, n); // Process noise covariance
    Eigen::MatrixXd R(m, m); // Measurement noise covariance
    Eigen::MatrixXd P(n, n); // Estimate error covariance
    Eigen::MatrixXd I(n, n);
    Eigen::MatrixXd x_hat(n,1);

    A << 1, 0, dt,  0, dt*dt/2,       0,
         0, 1,  0, dt,       0, dt*dt/2,
         0, 0,  1,  0,      dt,       0,
         0, 0,  0,  1,       0,      dt,
         0, 0,  0,  0,       1,       0,
         0, 0,  0,  0,       0,       1;

    C << 1, 0, 0, 0, 0, 0,
         0, 1, 0, 0, 0, 0;

    Q << 0.5, 0.5,   0,   0, 0, 0,
         0.5, 0.5,   0,   0, 0, 0,
           0,   0, 0.5, 0.5, 0, 0,
           0,   0, 0.5, 0.5, 0, 0,
           0,   0,   0,   0, 1, 0,
           0,   0,   0,   0, 0, 1;

    R << 5, 0,
         0, 5;

    P << 0.1, 0.1,  0.1,  0.1, 0.1, 0.1,
         0.1, 0.1,  0.1,  0.1, 0.1, 0.1,
         0.1, 0.1, 1000,  0.1,  10,  10,
         0.1, 0.1,  0.1, 1000,  10,  10,
         0.1, 0.1,   10,   10, 100, 0.1,
         0.1, 0.1,   10,   10, 0.1, 100;

    this->A = A;
    this->C = C;
    this->Q = Q;
    this->R = R;
    this->P = P;

    if(n>0) {
        Eigen::MatrixXd I(n, n);
        for(int i = 0; i< n; i++) {
            for(int j = 0; j< n; j++) {
                if(i==j) {
                    I(i,j) = 1;
                } else {
                    I(i,j) = 0;
                }

            }
        }
        this->I = I;
    }
    x_hat<<0,0,0,0,0,0;
    qDebug() << "-------------------------------------------------------TEST TEST TEST 20--------------------------------";
}

void FiltrKalmana::update(Eigen::MatrixXd y)
{
    Eigen::MatrixXd K(n, n);

    Eigen::MatrixXd x_hat_new(n,1);
    //Eigen::MatrixXd y(m,1);
    qDebug() << "-------------------------------------------------------TEST TEST TEST 21--------------------------------";
    x_hat_new = A * x_hat;
    P = A*P*A.transpose() + Q;
    K = P*C.transpose()*(C*P*C.transpose() + R).inverse();
    x_hat_new += K * (y - C*x_hat_new);
    P = (I - K*C)*P;
    x_hat = x_hat_new;
}

void FiltrKalmana::setX_hat(const Eigen::MatrixXd &value)
{
    x_hat = value;
}

Eigen::MatrixXd FiltrKalmana::getX_hat() const
{
    return x_hat;
}
