#ifndef DEFINE_H
#define DEFINE_H

#include <cmath>
#include <vector>
#include <stdint.h>

#include <QString>


const int ROBOT_DOF = 7;

// Status
const int STATUS_DEFAULTS = 100;
const int ENABLE = 101;
const int DISABLE = 102;
const int JSCTRL = 200;
const int CSCTRL = 300;
const int DEMO = 400;

// Control Command
const int COMMAND_DEFAULTS = 100;
const int ROBOT_ENABLE = 101;
const int ROBOT_DISABLE = 102;
const int ROBOT_STOP = 103;

const int JTS_BIAS = 104;
const int CD_ON = 105;
const int CD_OFF = 106;
const int FRICTION_OBSERVER = 107;
const int FRICTION_MODEL = 108;

const int JS_HG_ON = 110;
const int JS_HG_OFF = 111;
const int CS_HG_ON = 112;
const int CS_HG_OFF = 113;
const int CS_HG_X_ON = 114;
const int CS_HG_X_OFF = 115;
const int CS_HG_Y_ON = 116;
const int CS_HG_Y_OFF = 117;
const int CS_HG_Z_ON = 118;
const int CS_HG_Z_OFF = 119;

const int JS_FRICTION_OBSERVER = 107;
const int CS_IMPEDANCE_INITIATION = 316;

const int SET_DYN_PARA_GRIPPER = 121;
const int SET_DYN_PARA_POINTER = 122;
const int SET_DYN_PARA_PAYLOAD = 123;

const int JSCTRL_START = 200;
const int JS_TARGET_PATH = 201;

const int CSCTRL_START = 300;
const int CS_TARGET_PATH = 301;
const int CS_CIRCULAR_PATH = 317;

const int CS_IMPEDANCE_CTRL_ON = 310;
const int CS_IMPEDANCE_CTRL_OFF = 311;

const int CS_FORCE_CTRL_ON = 320;
const int CS_FORCE_CTRL_OFF = 321;
const int CS_FORCE_BIAS = 322;

const int DEMO1_START = 400;
const int DEMO1_END = 401;
const int DEMO2_START = 402;
const int DEMO2_END = 403;
const int DEMO3_START = 404;
const int DEMO3_END = 405;

const int GRIPPER_OPEN = 500;
const int GRIPPER_CLOSED = 501;

const int SET_TCP = 600;
const int SET_PAYLOAD = 601;
const int SET_SAFETY = 602;

const int COMMAND_ARCBLENDING = 800;
const int COMMAND_ARCBLENDING_ORIENTATION = 801;
const int COMMAND_ARCBLENDING_CHECK = 802;
const int COMMAND_ARCBLENDING_DIFFVEL = 803;

const int COMMAND_PARAMETER_TUNNER = 2000;

// math
const double RADPERDEG = M_PI / 180.0;
const double DEGTORAD = M_PI / 180.0;
const double RADTODEG = 180.0 / M_PI;
const double EPSILON = 0.0001;


struct RobotData {
    std::vector<double> measuredJointAngle;
    std::vector<double> measuredJointVelocity;
    std::vector<double> measuredJointTorque;
    std::vector<double> measuredCartesianPose;
    std::vector<double> measuredCartesianVelocity;
    std::vector<double> measuredForce;
    std::vector<double> desiredJointAngle;
    std::vector<double> desiredCartesianPose;
    uint32_t robotCommand;
    uint32_t robotStatus;
    bool collisionStatus;
    bool isPathOperating;

    std::vector<double> jointAnglePGain;
    std::vector<double> jointAngleIGain;
    std::vector<double> jointVelocityPGain;
    std::vector<double> jointVelocityIGain;
    std::vector<double> posePGain;
    std::vector<double> poseIGain;
    std::vector<double> velocityPGain;
    std::vector<double> velocityIGain;
    std::vector<double> impedanceM;
    std::vector<double> impedanceD;
    std::vector<double> impedanceK;

    RobotData () {
        measuredJointAngle.resize(ROBOT_DOF);
        measuredJointVelocity.resize(ROBOT_DOF);
        measuredJointTorque.resize(ROBOT_DOF);
        measuredCartesianPose.resize(6);
        measuredCartesianVelocity.resize(6);
        measuredForce.resize(6);
        desiredJointAngle.resize(ROBOT_DOF);
        desiredCartesianPose.resize(6);
        robotCommand = COMMAND_DEFAULTS;
        robotStatus = STATUS_DEFAULTS;
        isPathOperating = false;
        collisionStatus = false;

        jointAnglePGain.resize(ROBOT_DOF);
        jointAngleIGain.resize(ROBOT_DOF);
        jointVelocityPGain.resize(ROBOT_DOF);
        jointVelocityIGain.resize(ROBOT_DOF);
        posePGain.resize(ROBOT_DOF);
        poseIGain.resize(ROBOT_DOF);
        velocityPGain.resize(ROBOT_DOF);
        velocityIGain.resize(ROBOT_DOF);
        impedanceM.resize(ROBOT_DOF);
        impedanceD.resize(ROBOT_DOF);
        impedanceK.resize(ROBOT_DOF);
    }
};

const QString REGEX_NUM("\\[([\\d])\\]");

#endif // DEFINE_H
