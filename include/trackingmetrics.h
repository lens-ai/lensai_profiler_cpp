#ifndef TRACKING_METRICS_H
#define TRACKING_METRICS_H

#include <cmath>
#include <Eigen/Dense> 
#include <Eigen/Geometry>

using namespace Eigen;

struct Position2D {
    float x, y;
};

struct Position3D {
    float x, y, z;
};

struct AngularVelocity {
    float yawRate, pitchRate, rollRate;
};

class TrackingMetrics {
public:
    float computePositionError(const Position2D& predicted, const Position2D& actual);
    float computePositionError(const Position3D& predicted, const Position3D& actual);
    float computeOrientationError(const Quaternionf& predicted, const Quaternionf& actual);
    float computeAngularVelocityLatency(const AngularVelocity& measured, const AngularVelocity& expected);
    float computeQuaternionDrift(const Quaternionf& previous, const Quaternionf& current, float deltaTime);
    float computeCovarianceSpread(const Matrix3f& covarianceMatrix);
    float computeAngularDivergence(const Quaternionf& predicted, const Quaternionf& actual);
    bool detectAnomalousRotation(const AngularVelocity& current, const AngularVelocity& historicalAverage);
};

#endif // TRACKING_METRICS_H
