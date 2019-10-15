#ifndef TCP_MENAGER_HPP
#define TCP_MENAGER_HPP

#include <QObject>
#include <vector>

class Driver : public QObject {
    Q_OBJECT
public:
    Driver(QObject *parent = nullptr);

public:
    virtual void init() = 0;

    virtual void sendCommand(int16_t command) = 0;
    virtual void moveJointParameter(std::vector<double> q, std::vector<double> qd, std::vector<double> qdd) = 0;
    virtual void moveLinearParameter(std::vector<double> x, std::vector<double> xd, std::vector<double> xdd) = 0;
    virtual void moveCircularParameter(std::vector<std::vector<double>> poses, std::vector<double> xd, double accelerationTime) = 0;
    virtual void moveBlendingParameter(std::vector<std::vector<double>> poses, std::vector<std::vector<double>> xds, std::vector<std::vector<double>> radius, int16_t option, int16_t mode, double accelerationTime) = 0;

    virtual void setEnable(bool enable) = 0;
    virtual void stop() = 0;
    virtual void setRobotSpace(bool isJoint) = 0;
    virtual void setJointSpaceHandGuiding(bool turnOn) = 0;
    virtual void setCartesianSpaceHandGuiding(bool turnOn) = 0;
    virtual void setTcp(std::vector<double> tcp) = 0;
    virtual void setPayload(double payload) = 0;
    virtual void setForceMode(bool on, std::vector<double> target) = 0;
    virtual void setCollisionMode(bool on, double sensitivity) = 0;
    virtual void setCollisionStopResume() = 0;

    virtual std::vector<double> getMeasuredJointAngle() = 0;
    virtual std::vector<double> getMeasuredJointVelocity() = 0;
    virtual std::vector<double> getMeasuredJointTorque() = 0;
    virtual std::vector<double> getMeasuredCartesianPose() = 0;
    virtual std::vector<double> getMeasuredCartesianVelocity() = 0;
    virtual std::vector<double> getMeasuredForce() = 0;
    virtual std::vector<double> getDesiredJointAngle() = 0;
    virtual std::vector<double> getDesiredCartesianPose() = 0;
    virtual uint32_t getRobotCommand() = 0;
    virtual uint32_t getRobotStatus() = 0;
    virtual uint32_t getCollisionStatus() = 0;
    virtual bool isPathOperating() = 0;
};


#endif
