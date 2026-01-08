#include "ekf.h"

#define GRAVITY 9.81f

EKF::EKF() {
    init(0.0f);
}

void EKF::init(float ground_altitude) {
    z = ground_altitude;
    vz = 0.0f;
    P_z = 1.0f;
    P_vz = 0.1f;

    dt = 0.01f;     // 100 Hz
    Q_z = 0.01f;    // Position process noise
    Q_vz = 0.5f;    // Velocity process noise
    R_baro = 2.0f;  // Barometer measurement noise
}

void EKF::predict(float ax_g, float ay_g, float az_g) {
    // Vertical acceleration (remove 1g gravity)
    float az_world = (az_g - 1.0f) * GRAVITY;

    // Update velocity and position
    vz += az_world * dt;
    z += vz * dt;

    // Update covariance
    P_z += P_vz * dt * dt + Q_z * dt;
    P_vz += Q_vz * dt;
}

void EKF::updateBaro(float altitude_m) {
    // Kalman gain
    float K = P_z / (P_z + R_baro);

    // Update state
    float innovation = altitude_m - z;
    z += K * innovation;
    vz += 0.1f * K * innovation;  // Small velocity correction

    // Update covariance
    P_z *= (1.0f - K);

    // Bound covariance
    if(P_z < 0.001f) P_z = 0.001f;
    if(P_z > 100.0f) P_z = 100.0f;
    if(P_vz < 0.01f) P_vz = 0.01f;
    if(P_vz > 100.0f) P_vz = 100.0f;
}

float EKF::getAltitude() {
    return z;
}

float EKF::getVelocity() {
    return vz;
}
