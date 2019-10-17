#ifndef TCP_DRIVER_INTERFACE_HPP
#define TCP_DRIVER_INTERFACE_HPP

#include <QTcpSocket>
#include <QJsonObject>
#include <QMutex>
#include <atomic>

#include "src/define.h"
#include "driver.hpp"


class TcpDriverInterface : public Driver {
public:
    TcpDriverInterface(QObject *parent = nullptr);

public slots:
    void sendJson(QJsonObject json);

public:
    void init();

    void sendCommand(int16_t command);
    void moveJointParameter(std::vector<double> q, std::vector<double> qd, std::vector<double> qdd);
    void moveLinearParameter(std::vector<double> x, std::vector<double> xd, std::vector<double> xdd);
    void moveCircularParameter(std::vector<std::vector<double>> poses, std::vector<double> xd, double accelerationTime);
    void moveBlendingParameter(std::vector<std::vector<double>> poses, std::vector<std::vector<double>> xds, std::vector<std::vector<double>> radius, int16_t option, int16_t mode, double accelerationTime);

    void setEnable(bool enable);
    void stop();
    void setRobotSpace(bool isJoint);
    void setJointSpaceHandGuiding(bool turnOn);
    void setCartesianSpaceHandGuiding(bool turnOn);
    void setTcp(std::vector<double> tcp);
    void setPayload(double payload);
    void setForceMode(bool on, std::vector<double> target);
    void setCollisionMode(bool on, double sensitivity);
    void setCollisionStopResume();

    std::vector<double> getMeasuredJointAngle();
    std::vector<double> getMeasuredJointVelocity();
    std::vector<double> getMeasuredJointTorque();
    std::vector<double> getMeasuredCartesianPose();
    std::vector<double> getMeasuredCartesianVelocity();
    std::vector<double> getMeasuredForce();
    std::vector<double> getDesiredJointAngle();
    std::vector<double> getDesiredCartesianPose();
    uint32_t getRobotCommand();
    uint32_t getRobotStatus();
    uint32_t getCollisionStatus();
    bool isPathOperating();

    RobotData getRobotData();

private slots:
    void onConnected();
    void onDisconnected();
    void onUpdate();
    void startTimer();
    void stopTimer();

private:
    void waitForPathOperating();
    bool parseJsonFromBuffer(QJsonObject &json);
    std::vector<double> parseVectorFromJson(QJsonArray json);

private:
    QTcpSocket *socket;
    QByteArray buffer;
    RobotData robotData;
    std::atomic<bool> isStop;
    QMutex mutex;
};

#endif // TCP_DRIVER_INTERFACE_HPP
