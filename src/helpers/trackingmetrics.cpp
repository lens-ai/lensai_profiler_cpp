#include "trackingmetrics.h"
#include <iostream>
#include <cmath>

using namespace Eigen;

// 2D Position Error Calculation
float TrackingMetrics::computePositionError(const Position2D& predicted, const Position2D& actual) {
    return sqrt(pow(predicted.x - actual.x, 2) + pow(predicted.y - actual.y, 2));
}

// 3D Position Error Calculation
float TrackingMetrics::computePositionError(const Position3D& predicted, const Position3D& actual) {
    return sqrt(pow(predicted.x - actual.x, 2) + pow(predicted.y - actual.y, 2) + pow(predicted.z - actual.z, 2));
}

// Quaternion Orientation Error Calculation
float TrackingMetrics::computeOrientationError(const Quaternionf& predicted, const Quaternionf& actual) {
    Quaternionf error = predicted * actual.conjugate();
    return 2.0f * acos(fabs(error.w()));
}

// Angular Velocity Latency Calculation
float TrackingMetrics::computeAngularVelocityLatency(const AngularVelocity& measured, const AngularVelocity& expected) {
    float yawDiff = fabs(measured.yawRate - expected.yawRate);
    float pitchDiff = fabs(measured.pitchRate - expected.pitchRate);
    float rollDiff = fabs(measured.rollRate - expected.rollRate);
    return (yawDiff + pitchDiff + rollDiff) / 3.0f;
}

// Quaternion Drift Calculation
float TrackingMetrics::computeQuaternionDrift(const Quaternionf& previous, const Quaternionf& current, float deltaTime) {
    Quaternionf deltaQ = current * previous.conjugate();
    float angle = 2.0f * acos(fabs(deltaQ.w()));
    return angle / deltaTime;
}

// Covariance Spread Calculation
float TrackingMetrics::computeCovarianceSpread(const Matrix3f& covarianceMatrix) {
    return covarianceMatrix.trace();  // Sum of the variances for each axis
}

// Angular Divergence Calculation
float TrackingMetrics::computeAngularDivergence(const Quaternionf& predicted, const Quaternionf& actual) {
    Quaternionf error = predicted * actual.conjugate();
    return acos(fabs(error.w()));
}

// Anomalous Rotation Detection
bool TrackingMetrics::detectAnomalousRotation(const AngularVelocity& current, const AngularVelocity& historicalAverage) {
    const float threshold = 0.5f;  // Threshold for anomaly detection (example)
    float yawAnomaly = fabs(current.yawRate - historicalAverage.yawRate) > threshold;
    float pitchAnomaly = fabs(current.pitchRate - historicalAverage.pitchRate) > threshold;
    float rollAnomaly = fabs(current.rollRate - historicalAverage.rollRate) > threshold;
    return yawAnomaly || pitchAnomaly || rollAnomaly;
}
