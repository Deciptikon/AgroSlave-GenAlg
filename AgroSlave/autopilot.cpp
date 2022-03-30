#include "autopilot.h"

Autopilot::Autopilot(QObject *parent) : QObject(parent)
{
    isValidNeuroNet = false;
}

void Autopilot::init(const int msecDeltaTime)
{
    if(msecDeltaTime > 1) {
        this->msecDeltaTime = msecDeltaTime;
    } else {
        this->msecDeltaTime = 1;
    }
    velocity        = QVector3D(0,0,0);
    translate       = QVector3D(0,0,0);
    angle           = QVector3D(0,0,0);

    //initV0();//первая версия оабучения алгоритма
    initV1();
}

void Autopilot::loop()
{
    //qDebug() << "void Autopilot::loop()-------------------------------------------------";

    //update velocity and translate
    velocity += accelerometer*(float)msecDeltaTime/1000.0;
    translate+= velocity     *(float)msecDeltaTime/1000.0;
//    qDebug() << "void Autopilot::accelerometer =" << accelerometer;
//    qDebug() << "void Autopilot::velocity      =" << velocity;
//    qDebug() << "void Autopilot::translate     =" << translate;
    accelerometer = QVector3D(0,0,0);


    angle    += gyroscope    *(float)msecDeltaTime/1000.0;
    qDebug() << "void Autopilot::gyroscope =" << gyroscope;
    qDebug() << "void Autopilot::angle     =" << angle;
    gyroscope     = QVector3D(0,0,0);

    //runV0();//первая версия обучения алгоритма
    runV1();
}

void Autopilot::readFromSensorReader(const QVector3D &accel, const QVector3D &gyro)
{
    //qDebug() << "Autopilot::readFromSensorReader(const QVector3D &accel, const QVector3D &gyro)";
    //QVector3D a={0,0,0};

    //qDebug() << "updateSensors Accelerometer:   " << accel;
    //qDebug() << "updateSensors Gyroscope:       " << gyro ;
    if(!accel.isNull()) {
        accelerometer += accel;
        //velocity += accelerometer*(float)msecDeltaTime/1000.0;
    }
    if(!gyro.isNull()) {
        gyroscope += gyro;
        //qDebug() << "updateSensors Gyroscope:   " << gyroscope ;
        //qDebug() << "updateSensors angle =      " << angle;
    }
}

void Autopilot::readFromBluetoothServer(const QByteArray &message)
{
    qDebug() << "Autopilot::readFromBluetoothServer(const QByteArray &message)";

    if(message.size()==1) {
            //qDebug() << "TEST";
            //QByteArray b;
            char p=message.at(0);
            switch (p) {
            case '4':

                break;
            case '5':
                //
                break;
            case '6':
                //
                break;
            case '7'://меняем состояние реле
                restateRelay();
                break;
            case '8':
                //
                break;


            default:
                qDebug() << "default: " << (int)p;
                break;
            }
        } else {//проверяем команду с параметрами
//            char p=message.at(0);
//            switch (p) {
//            case 0x01:
//                processKeyPoint(message);
//                break;

//            case 0x03:
//                b.append(message);
//                b.append('\n');
//                qDebug() << "Rotate: " << b.toHex();
//                sendToArduino(b);
//                break;


//            default:
//                qDebug() << "default: " << (int)p;
//                break;
//            }
        }
}

void Autopilot::readFromGPS(const double &x, const double &y)
{
    qDebug() << "Autopilot::readFromGPS(double &x, double &y)";
}

void Autopilot::resetDataSens()
{
    accelerometer = QVector3D(0,0,0);
    velocity      = QVector3D(0,0,0);
    translate     = QVector3D(0,0,0);
    gyroscope     = QVector3D(0,0,0);
    angle         = QVector3D(0,0,0);
}

void Autopilot::startLearn()
{
    onLearn = true;
}

void Autopilot::stopLearn()
{
    onLearn = false;
}

void Autopilot::reverseLearn()
{
    onLearn = !onLearn;

    if (onLearn) {
        //isValidNeuroNet = false;
        currentTraining = 0;
        epoha = 0;
    } else {
        //
    }

    qDebug() << "void Autopilot::reverseLearn() -->" << onLearn;
}

float Autopilot::angleGAMATestForNN(float angle)
{
    //return (angle*0.85)*(angle*0.85);
    //return angle/2.0f;
    return sin(angle*M_PI/2.0);
}

float Autopilot::angleALFATestForNN(float maxAngle, int maxNum, int num)
{
    float step = 0;
    float ang  = -maxAngle;
    if(num>=0 && maxNum>1) {
        step = 2*maxAngle/(maxNum - 1);
        ang = -maxAngle + num*step;
    }
    if(ang > maxAngle) {
        ang  = maxAngle;
    } else if(ang < -maxAngle) {
        ang  =-maxAngle;
    }
    return ang;
}

void Autopilot::acceptNewMaxValueMutation(const double &value)
{
    //qDebug() << "void Autopilot::acceptNewMaxValueMutation(const double &value) =" << value;
    maxValueMutation = value;
    gen.setMaxValueMutation(value);
}

void Autopilot::acceptNewProbalityMutation(const double &value)
{
    probalityMutation = value;
    gen.setProbalityMutation(value);
}

void Autopilot::acceptNewGeneration(const double &value)
{
    generateMutation = value;
    gen.setGenerateMutation(value);
}

void Autopilot::acceptNeuroNet(const NeuroNet net)
{
    qDebug() << "void Autopilot::acceptNeuroNet(const NeuroNet net)";
    this->neuroNet = net;
    if(!this->neuroNet.isEmpty()) {
        isValidNeuroNet = true;
        calculateNeuroTopology();
        epoha += 2;
        emit currentLern(epoha, this->neuroNet.getAdaption());
    }
}

void Autopilot::initV0()
{


    MAX_TRAIN       = 10;//максимальное количество тренировок для особи
    MAX_COUNT       = 5;//количество базовых особей в популяции

    count           = 0;
    epoha           = 0;
    currentTraining = -1;
    err             = 0;
    onLearn         = false;

    generateMutation  = 10;
    maxValueMutation  = 1;
    probalityMutation = 0.1;

    net = NeuroNet({1,5,1}, 10);//init neuro net

    gen.init(MAX_COUNT, net);
    gen.setGenerateMutation(generateMutation);
    gen.setMaxValueMutation(maxValueMutation);
    gen.setProbalityMutation(probalityMutation);
    //TEST NEURO
    alfa = 0;
    beta = 0;
    gama = 0;
}

void Autopilot::runV0()
{
    if(onLearn) {
        for(int i=0; i<MAX_COUNT*5*MAX_TRAIN; i++) {//=======>>>
        if(currentTimeDrive >= calculateTimeDrive) {
            //qDebug() << "void Autopilot::loop()-------------------------------------------------";
            angleB = angle;

/*            QVector3D angle -> to float floatAngle // oschibka upravleniya, tchem men'sche, tem lutchsche
            float ang;
            ang = (angleB - angleA).length();*/

            //-----------------------------------------------------------------------
            ///-------DRIVING---------
            if(currentTraining != -1) {
                gama = angleGAMATestForNN(beta);//opredelyaem povorot
                //qDebug() << "gama     =" << gama;

/*                if(alfa == 0) {
                    err += 1000;
                } else {
                    err += (alfa - gama)*(alfa - gama);//otklonenie
                }*/
                err += (alfa - gama)*(alfa - gama);//otklonenie
                //qDebug() << "err      =" << err;
            }
            //-----------------------------------------------------------------------


            angleA = angleB;

            currentTraining++;
            emit currentTrain(currentTraining);

            if(currentTraining == MAX_TRAIN) {
                //-----------------------------------------------------------------------
                gen.update(count, err);//prisposoblennost' eto kvadrat oschibki
                //--------------------------------------------------------------------------

                currentTraining = -1;
                count++;
                emit currentCount(count);
                //qDebug() << "";
                //qDebug() << "===== Count =" << count << " =====";
                err  = 0;
                alfa = 0;
                beta = 0;
                gama = 0;
            } else {
                //-----------------------------------------------------------------------------
                //commAngleRand = rndAngle();
                //commAngleNeuro = gen.getPopulation.at(count).getNeuro(commAngleRand)
//                alfa = QRandomGenerator::global()->bounded(2.0)-1.0;//jelaemiy ugol povorota
                alfa = angleALFATestForNN(1.0, MAX_TRAIN, currentTraining);//jelaemiy ugol povorota
                //qDebug() << "alfa     =" << alfa;
                NeuroNet nn = gen.getPopulation().at(count);
                QList<float> dataNet{alfa};
                nn.run(dataNet);
                beta = nn.getSelfValues().first()*2.0f;//korrekciya neuroseti
                //qDebug() << "beta     =" << beta;


                calculateTimeDrive = 0;//update time drive >0
                ///command to drive --> --> --> f(beta)-->


                currentTimeDrive = 0;
                //------------------------------------------------------------------------------
            }

            if(count == MAX_COUNT*5) {
                gen.run();
                count = 0;
                net = gen.getPopulation().first();//обновление текущей сети лучшей из обученных
                calculateNeuroTopology();

                const float x = epoha;
                const float y = gen.getPopulation().first().getAdaption();
                //qDebug() << "x     =" << x;
                //qDebug() << "y     =" << y;
//                qDebug() << "maxValueMutation  =" << maxValueMutation;
//                qDebug() << "probalityMutation =" << probalityMutation;
//                qDebug() << "generateMutation  =" << generateMutation;
                emit currentError(y);
                emit currentLern(x, y);

                epoha++;
                emit currentEpoha(epoha);
                //qDebug() << "";
                //qDebug() << "";
                //qDebug() << "////////////////////////////// EPOHA =" << epoha << " //////////////////////////////////";
                //qDebug() << "//";
                //qDebug() << "//";

            }

        } else {
            currentTimeDrive += msecDeltaTime;
        }
        }//=======>>>
    }
}

void Autopilot::initV1()
{
    onLearn = false;

    MAX_TRAIN = 20;
    currentTraining = 0;

    angleA = {0,0,0};
    angleB = {0,0,0};

    alfa = 0;
    beta = 0;

    currentTimeDrive=1;
    calculateTimeDrive=0;

    neuroNet = NeuroNet({1,5,5,1}, 1);//инициализация нейронной сети
}

void Autopilot::runV1()
{
    if(onLearn && currentTraining<MAX_TRAIN) {
        if(currentTimeDrive > calculateTimeDrive) {

            angleA = angleB;

            angleB = angle;

            if(currentTraining != 0) {
                //beta = (angleB-angleA).length();//угол на который довернулся
                beta = angleGAMATestForNN(alfa);//тестовая версия
qDebug() << "currentTraining" << currentTraining;
qDebug() << "alfa" << alfa;
qDebug() << "beta" << beta;
                inputAngle.append(alfa);
                outputAngle.append(beta);

            }

            alfa = angleALFATestForNN(1.0, MAX_TRAIN, currentTraining);//желаемый угол поворота

            //коррекция при помощи нейросети (обученной)
            //gama = ????
            if(isValidNeuroNet) {
                //gama = neuroNet.run({alfa}).getSelfValues.first();
                gama = alfa;
            } else {
                gama = alfa;
            }





            // поворачиваем //

            // преобразуем угол в команду
            // int comm = func(gama);

            // отправляем команду
            //sendCommandToSlave14(int &comm)

            currentTimeDrive = 0;
            calculateTimeDrive = 1500;///--- calculateTimeDrive = func2(gama);  ---------------------

            currentTraining++;
            emit currentTrain(currentTraining);

        } else {
            currentTimeDrive += msecDeltaTime;
        }
    } else if(currentTraining == MAX_TRAIN) {
        qDebug() << "currentTraining == MAX_TRAIN";

        if(!isValidNeuroNet) {
            //отправляем сигнал с данными inputAngle and outputAngle и текущей нейросетью
            emit sendInputOutputDataAndNeuroNet(outputAngle, inputAngle, neuroNet);
        }

        currentTraining++;
    }
}

int Autopilot::getMSecDeltaTime() const
{
    return msecDeltaTime;
}

void Autopilot::calculateNeuroTopology()
{
    if(!neuroNet.isEmpty()) {
        QList<float> topology;
        int iter = 100;
        float s  = 2.0f/((float)iter);
        for(int i=0; i<iter; i++) {
            float ang = s*((float)i) - 1.0f;
            //qDebug() << "ANGLE =" << ang;
            QList<float> d{ang};
            neuroNet.run(d);
            float t = neuroNet.getSelfValues().first();
            //qDebug() << "REZ =" << t;
            topology.append(t);
        }
        emit currentNeuroTopology(topology);
    }
}

void Autopilot::restateRelay()
{
    stateRelay = !stateRelay;
    qDebug() << "Состояние реле изменено на " << stateRelay;

    int c='c';
    emit sendCommandToSlave14(c);
}
