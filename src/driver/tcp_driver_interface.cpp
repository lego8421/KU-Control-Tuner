#include "tcp_driver_interface.hpp"

#include <QThread>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTimer>
#include <QTime>


TcpDriverInterface::TcpDriverInterface(QObject *parent)
    : Driver(parent)  {

    socket = new QTcpSocket(this);
}

void TcpDriverInterface::init() {

    QString address = "127.0.0.1";
    quint16 port = 8421;

    socket->connectToHost(address, port);

    connect(socket, &QTcpSocket::connected, this, &TcpDriverInterface::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &TcpDriverInterface::onDisconnected);
    //    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), [ = ](QAbstractSocket::SocketError socketError) {
    //        // reconnect
    //        if (socketError == QAbstractSocket::ConnectionRefusedError) {
    //            QTimer::singleShot(2000, [ = ]() {
    //                if (socket->state() != QAbstractSocket::ConnectedState) {
    //                    socket->connectToHost(address, 8421);
    //                }
    //            });
    //        }
    //    });
}


void TcpDriverInterface::sendCommand(int16_t command) {
    QJsonObject json;
    json["command"] = command;
    sendJson(json);
}

void TcpDriverInterface::moveJointParameter(std::vector<double> q, std::vector<double> qd, std::vector<double> qdd) {
    QJsonObject json;
    QJsonArray angle;
    QJsonArray velocity;
    QJsonArray acceleration;

    for (std::size_t i = 0; i < ROBOT_DOF; i++) {
        angle.push_back(q[i]);
        velocity.push_back(qd[i]);
        acceleration.push_back(qdd[i]);
    }

    json["command"] = JS_TARGET_PATH;
    json["targetQ"] = angle;
    json["targetQd"] = velocity;
    json["targetQdd"] = acceleration;

    setRobotSpace(true);
    sendJson(json);
    waitForPathOperating();
}

void TcpDriverInterface::moveLinearParameter(std::vector<double> x, std::vector<double> xd, std::vector<double> xdd) {
    QJsonObject json;
    QJsonArray pose;
    QJsonArray velocity;
    QJsonArray acceleration;

    for (std::size_t i = 0; i < 6; i++) {
        pose.push_back(x[i]);
        velocity.push_back(xd[i]);
        acceleration.push_back(xdd[i]);
    }

    json["command"] = CS_TARGET_PATH;
    json["targetX"] = pose;
    json["targetXd"] = velocity;
    json["targetXdd"] = acceleration;

    setRobotSpace(false);
    sendJson(json);
    waitForPathOperating();
}

void TcpDriverInterface::moveCircularParameter(std::vector<std::vector<double> > poses, std::vector<double> xd, double accelerationTime) {
    QJsonObject json;
    QJsonArray pose1;
    QJsonArray pose2;
    QJsonArray pose3;
    QJsonArray velocity;

    for (std::size_t i = 0; i < 6; i++) {
        pose1.push_back(poses[0][i]);
        pose2.push_back(poses[1][i]);
        pose3.push_back(poses[2][i]);
        velocity.push_back(xd[i]);
    }

    json["command"] = CS_CIRCULAR_PATH;
    json["circularPose1"] = pose1;
    json["circularPose2"] = pose2;
    json["circularPose3"] = pose3;
    json["targetXd"] = velocity;
    json["accelerationTime"] = accelerationTime;

    setRobotSpace(false);
    sendJson(json);
    waitForPathOperating();
}

void TcpDriverInterface::moveBlendingParameter(std::vector<std::vector<double>> poses, std::vector<std::vector<double>> xds, std::vector<std::vector<double>> radius, int16_t option, int16_t mode, double accelerationTime) {
    QJsonObject json;
    std::size_t num = poses.size();
    QJsonArray pose;
    QJsonArray velocity;
    QJsonArray r;
    for (std::size_t i = 0; i < num; i++) {
        QJsonArray poseTemp;
        QJsonArray velocityTemp;
        QJsonArray radiusTemp;
        for (std::size_t j = 0; j < 6; j++) {
            poseTemp.push_back(poses[i][j]);
        }
        pose.push_back(poseTemp);

        if (i < num - 2) {
            for (std::size_t j = 0; j < 2; j++) {
                radiusTemp.push_back(radius[i][j]);
                velocityTemp.push_back(xds[i][j]);
            }

            if (option == 2) {
                radiusTemp[1] = 30.0;
            }
            velocity.push_back(velocityTemp);
            r.push_back(radiusTemp);
        }
    }

    json["command"] = COMMAND_ARCBLENDING_ORIENTATION;
    json["blendingPose"] = pose;
    json["blendingVelocity"] = velocity;
    json["blendingRadius"] = r;
    json["blendingOption"] = option;
    json["blendingMode"] = mode;
    json["blendingCount"] = static_cast<int>(num);
    json["accelerationTime"] = accelerationTime;

    setRobotSpace(false);
    sendJson(json);
    waitForPathOperating();
}

void TcpDriverInterface::setEnable(bool enable) {
    int16_t command = enable ? ENABLE : DISABLE;
    sendCommand(command);
}

void TcpDriverInterface::stop() {
    int16_t command = ROBOT_STOP;
    sendCommand(command);
}

void TcpDriverInterface::setRobotSpace(bool isJoint) {
    int16_t space = isJoint ? JSCTRL_START : CSCTRL_START;
    sendCommand(space);
}

void TcpDriverInterface::setJointSpaceHandGuiding(bool turnOn) {
    int16_t command = turnOn ? JS_HG_ON : JS_HG_OFF;

    if (turnOn) {
        setRobotSpace(true);
    }
    sendCommand(command);
}

void TcpDriverInterface::setCartesianSpaceHandGuiding(bool turnOn) {
    int16_t command = turnOn ? CS_HG_ON : CS_HG_OFF;
    sendCommand(command);

    if (!turnOn) {
        setJointSpaceHandGuiding(true);
        setJointSpaceHandGuiding(false);
    }
}

void TcpDriverInterface::setTcp(std::vector<double> tcp) {
    uint32_t status = getRobotStatus();

    if (status == CSCTRL) {
        setRobotSpace(true);
    }

    QJsonObject json;
    QJsonArray t;

    for (std::size_t i = 0; i < 6; i++) {
        t.push_back(tcp[i]);
    }

    json["command"] = SET_TCP;
    json["tcp"] = t;
    sendJson(json);
}

void TcpDriverInterface::setPayload(double payload) {
    uint32_t status = getRobotStatus();

    if (status == CSCTRL) {
        setRobotSpace(true);
    }

    QJsonObject json;

    json["command"] = SET_PAYLOAD;
    json["payload"] = payload;
    sendJson(json);
}

void TcpDriverInterface::setForceMode(bool on, std::vector<double> target) {
    QJsonObject json;

    json["command"] = on ? CS_FORCE_CTRL_ON : CS_FORCE_CTRL_OFF;

    if (on) {
        QJsonArray array;
        for (std::size_t i = 0; i < 6; i++) {
            array.push_back(target[i]);
        }
        json["desiredForce"] = array;
    }

    sendJson(json);
}

void TcpDriverInterface::setCollisionMode(bool on, double sensitivity) {
    QJsonObject json;
    json["command"] = on ? CD_ON : CD_OFF;

    if (on) {
        json["collisionSensitivity"] = sensitivity;
    }

    sendJson(json);
}

void TcpDriverInterface::setCollisionStopResume() {
    QJsonObject json;
    json["collisionClear"] = true;
    sendJson(json);
}

std::vector<double> TcpDriverInterface::getMeasuredJointAngle() {
    std::vector<double> ret;

    mutex.lock();
    ret = robotData.measuredJointAngle;
    mutex.unlock();

    return ret;
}

std::vector<double> TcpDriverInterface::getMeasuredJointVelocity() {
    std::vector<double> ret;

    mutex.lock();
    ret = robotData.measuredJointVelocity;
    mutex.unlock();

    return ret;
}

std::vector<double> TcpDriverInterface::getMeasuredJointTorque() {
    std::vector<double> ret;

    mutex.lock();
    ret = robotData.measuredJointTorque;
    mutex.unlock();

    return ret;
}

std::vector<double> TcpDriverInterface::getMeasuredCartesianPose() {
    std::vector<double> ret;

    mutex.lock();
    ret = robotData.measuredCartesianPose;
    mutex.unlock();

    return ret;
}

std::vector<double> TcpDriverInterface::getMeasuredCartesianVelocity() {
    std::vector<double> ret;

    mutex.lock();
    ret = robotData.measuredCartesianVelocity;
    mutex.unlock();

    return ret;
}

std::vector<double> TcpDriverInterface::getMeasuredForce() {
    std::vector<double> ret;

    mutex.lock();
    ret = robotData.measuredForce;
    mutex.unlock();

    return ret;
}

std::vector<double> TcpDriverInterface::getDesiredJointAngle() {
    std::vector<double> ret;

    mutex.lock();
    ret = robotData.desiredJointAngle;
    mutex.unlock();

    return ret;
}

std::vector<double> TcpDriverInterface::getDesiredCartesianPose() {
    std::vector<double> ret;

    mutex.lock();
    ret = robotData.desiredCartesianPose;
    mutex.unlock();

    return ret;
}

uint32_t TcpDriverInterface::getRobotCommand() {
    uint32_t ret;

    mutex.lock();
    ret = robotData.robotCommand;
    mutex.unlock();

    return ret;
}

uint32_t TcpDriverInterface::getRobotStatus() {
    uint32_t ret;

    mutex.lock();
    ret = robotData.robotStatus;
    mutex.unlock();

    return ret;
}

uint32_t TcpDriverInterface::getCollisionStatus() {
    uint32_t ret;

    mutex.lock();
    ret = robotData.collisionStatus;
    mutex.unlock();

    return ret;
}

bool TcpDriverInterface::isPathOperating() {
    bool ret;

    mutex.lock();
    ret = robotData.isPathOperating;
    mutex.unlock();

    return ret;
}

RobotData TcpDriverInterface::getRobotData() {
    RobotData ret;

    mutex.lock();
    ret = robotData;
    mutex.unlock();

    return ret;
}

void TcpDriverInterface::onConnected() {
    startTimer();
}

void TcpDriverInterface::onDisconnected() {
    stopTimer();
    socket->close();
    buffer.clear();
}

void TcpDriverInterface::onUpdate() {
    if (socket->state() != QAbstractSocket::ConnectedState) {
        stopTimer();
        return;
    }

    socket->waitForReadyRead(1);
    if (socket->bytesAvailable()) {
        buffer += socket->readAll();
    }

    QJsonObject receivedJson;
    mutex.lock();
    while (parseJsonFromBuffer(receivedJson)) {
        if (receivedJson.keys().indexOf("actualQ") != -1) {
            robotData.measuredJointAngle = parseVectorFromJson(receivedJson["actualQ"].toArray());
        }
        if (receivedJson.keys().indexOf("actualQd") != -1) {
            robotData.measuredJointVelocity = parseVectorFromJson(receivedJson["actualQd"].toArray());
        }
        if (receivedJson.keys().indexOf("actualTorque") != -1) {
            robotData.measuredJointTorque = parseVectorFromJson(receivedJson["actualTorque"].toArray());
        }
        if (receivedJson.keys().indexOf("actualX") != -1) {
            robotData.measuredCartesianPose = parseVectorFromJson(receivedJson["actualX"].toArray());
        }
        if (receivedJson.keys().indexOf("actualXd") != -1) {
            robotData.measuredCartesianVelocity = parseVectorFromJson(receivedJson["actualXd"].toArray());
        }
        if (receivedJson.keys().indexOf("actualForce") != -1) {
            robotData.measuredForce = parseVectorFromJson(receivedJson["actualForce"].toArray());
        }
        if (receivedJson.keys().indexOf("desiredQ") != -1) {
            robotData.desiredJointAngle = parseVectorFromJson(receivedJson["desiredQ"].toArray());
        }
        if (receivedJson.keys().indexOf("desiredX") != -1) {
            robotData.desiredCartesianPose = parseVectorFromJson(receivedJson["desiredX"].toArray());
        }
        if (receivedJson.keys().indexOf("status") != -1) {
            robotData.robotStatus = receivedJson["status"].toInt(STATUS_DEFAULTS);
        }
        if (receivedJson.keys().indexOf("command") != -1) {
            robotData.robotCommand = receivedJson["command"].toInt(COMMAND_DEFAULTS);
        }
        if (receivedJson.keys().indexOf("collisionStatus") != -1) {
            robotData.collisionStatus = receivedJson["collisionStatus"].toBool(false);
        }
        if (receivedJson.keys().indexOf("isPathOperating") != -1) {
            robotData.isPathOperating = receivedJson["isPathOperating"].toBool(false);
        }

        if (receivedJson.keys().indexOf("jointAnglePGain") != -1) {
            robotData.jointAnglePGain = parseVectorFromJson(receivedJson["jointAnglePGain"].toArray());
        }
        if (receivedJson.keys().indexOf("jointAngleIGain") != -1) {
            robotData.jointAngleIGain = parseVectorFromJson(receivedJson["jointAngleIGain"].toArray());
        }
        if (receivedJson.keys().indexOf("jointVelocityPGain") != -1) {
            robotData.jointAngleIGain = parseVectorFromJson(receivedJson["jointVelocityPGain"].toArray());
        }
        if (receivedJson.keys().indexOf("jointVelocityIGain") != -1) {
            robotData.jointAngleIGain = parseVectorFromJson(receivedJson["jointVelocityIGain"].toArray());
        }
        if (receivedJson.keys().indexOf("posePGain") != -1) {
            robotData.posePGain = parseVectorFromJson(receivedJson["posePGain"].toArray());
        }
        if (receivedJson.keys().indexOf("poseIGain") != -1) {
            robotData.poseIGain = parseVectorFromJson(receivedJson["poseIGain"].toArray());
        }
        if (receivedJson.keys().indexOf("velocityPGain") != -1) {
            robotData.velocityPGain = parseVectorFromJson(receivedJson["velocityPGain"].toArray());
        }
        if (receivedJson.keys().indexOf("velocityIGain") != -1) {
            robotData.velocityIGain = parseVectorFromJson(receivedJson["velocityIGain"].toArray());
        }
        if (receivedJson.keys().indexOf("impedanceM") != -1) {
            robotData.impedanceM = parseVectorFromJson(receivedJson["impedanceM"].toArray());
        }
        if (receivedJson.keys().indexOf("impedanceD") != -1) {
            robotData.impedanceD = parseVectorFromJson(receivedJson["impedanceD"].toArray());
        }
        if (receivedJson.keys().indexOf("impedanceK") != -1) {
            robotData.impedanceK = parseVectorFromJson(receivedJson["impedanceK"].toArray());
        }
    }
    mutex.unlock();

    if (!isStop) {
        QTimer::singleShot(0, this, &TcpDriverInterface::onUpdate);
    }
}

void TcpDriverInterface::startTimer() {
    isStop.store(false);
    QTimer::singleShot(1, this, &TcpDriverInterface::onUpdate);
}

void TcpDriverInterface::stopTimer() {
    isStop.store(true);
}

void TcpDriverInterface::waitForPathOperating() {
    int timeout = 1000;
    while(true) {
        if (isPathOperating()) {
            break;
        }
        if (timeout-- < 0) {
            break;
        }
        QThread::msleep(1);
    }
}

bool TcpDriverInterface::parseJsonFromBuffer(QJsonObject &json) {
    int index = buffer.indexOf('{');
    if (index == -1) {
        buffer.clear();
        return false;
    }
    buffer.remove(0, index);

    index = buffer.indexOf('}');
    if (index == -1) {
        return false;
    }

    QByteArray data = buffer.mid(0, index + 1);
    buffer.remove(0, index + 1);

    QJsonDocument itemDoc = QJsonDocument::fromJson(data);
    json = itemDoc.object();

    return true;
}

std::vector<double> TcpDriverInterface::parseVectorFromJson(QJsonArray json) {
    std::vector<double> ret;
    for (int i = 0; i < json.size(); i++) {
        ret.push_back(json[i].toDouble(0.0));
    }
    return ret;
}

void TcpDriverInterface::sendJson(QJsonObject json) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        QJsonDocument doc(json);
        socket->write(doc.toJson(QJsonDocument::Compact));
        socket->flush();
        qDebug() << json;
    }
}
