#include "trackingmetrics.h"
#include <gtest/gtest.h>

// 2D and 3D Position Testing
TEST(TrackingMetricsTest, PositionError2D) {
    TrackingMetrics tracker;
    Position2D predicted = {10.0f, 20.0f};
    Position2D actual = {12.0f, 24.0f};
    float error = tracker.computePositionError(predicted, actual);
    EXPECT_NEAR(error, 5.6569f, 0.0001f);  // Tolerance added for precision
}

TEST(TrackingMetricsTest, PositionError3D) {
    TrackingMetrics tracker;
    Position3D predicted = {10.0f, 20.0f, 5.0f};
    Position3D actual = {12.0f, 24.0f, 7.0f};
    float error = tracker.computePositionError(predicted, actual);
    EXPECT_NEAR(error, 5.6569f, 0.0001f);  // Tolerance added
}

// Quaternion Orientation Testing
TEST(TrackingMetricsTest, OrientationError) {
    TrackingMetrics tracker;
    Quaternion predicted = {1.0f, 0.0f, 0.0f, 0.0f}; // Identity quaternion
    Quaternion actual = {0.707f, 0.707f, 0.0f, 0.0f}; // 90-degree rotation
    float error = tracker.computeOrientationError(predicted, actual);
    EXPECT_NEAR(error, 1.5708f, 0.0001f);  // Tolerance for radians
}

// Angular Velocity Latency Testing
TEST(TrackingMetricsTest, AngularVelocityLatency) {
    TrackingMetrics tracker;
    AngularVelocity measured = {2.0f, 3.0f, 1.0f};
    AngularVelocity expected = {2.1f, 3.1f, 1.1f};
    float latency = tracker.computeAngularVelocityLatency(measured, expected);
    EXPECT_NEAR(latency, 0.1f, 0.0001f);  // Average of absolute differences
}

// Quaternion Drift Testing
TEST(TrackingMetricsTest, QuaternionDrift) {
    TrackingMetrics tracker;
    Quaternion previous = {1.0f, 0.0f, 0.0f, 0.0f};
    Quaternion current = {0.707f, 0.707f, 0.0f, 0.0f};
    float drift = tracker.computeQuaternionDrift(previous, current, 1.0f);
    EXPECT_NEAR(drift, 1.5708f, 0.0001f);  // Drift for 1-second interval
}

// Covariance Spread Testing
TEST(TrackingMetricsTest, CovarianceSpread) {
    TrackingMetrics tracker;
    Matrix3f covarianceMatrix;
    covarianceMatrix << 1.0f, 0.5f, 0.3f,
                        0.5f, 1.0f, 0.4f,
                        0.3f, 0.4f, 1.0f;
    float spread = tracker.computeCovarianceSpread(covarianceMatrix);
    EXPECT_NEAR(spread, 3.0f, 0.0001f);  // Trace of covariance matrix
}

// Angular Divergence Testing
TEST(TrackingMetricsTest, AngularDivergence) {
    TrackingMetrics tracker;
    Quaternion predicted = {1.0f, 0.0f, 0.0f, 0.0f};
    Quaternion actual = {0.707f, 0.707f, 0.0f, 0.0f};
    float divergence = tracker.computeAngularDivergence(predicted, actual);
    EXPECT_NEAR(divergence, 1.5708f, 0.0001f);
}

// Anomalous Rotation Detection Testing
TEST(TrackingMetricsTest, DetectAnomalousRotation) {
    TrackingMetrics tracker;
    AngularVelocity current = {5.0f, 0.0f, 0.0f};
    AngularVelocity historicalAverage = {2.0f, 0.0f, 0.0f};
    bool isAnomalous = tracker.detectAnomalousRotation(current, historicalAverage);
    EXPECT_TRUE(isAnomalous);
}

// General Test for Metrics Logging (Placeholder)
TEST(TrackingMetricsTest, LogMetrics) {
    //TrackingMetrics tracker;
    //tracker.logMetrics();  // Placeholder for testing logging functionality
}

// Method Invocation and Logging Testing
TEST(TrackingMetricsTest, MethodInvocation) {
    TrackingMetrics tracker;
    Position2D predicted = {10.0f, 20.0f};
    Position2D actual = {12.0f, 24.0f};
    float error = tracker.computePositionError(predicted, actual);
    ASSERT_NEAR(error, 5.6569f, 0.0001f);
    //tracker.logMetrics();  // Placeholder check for logging
}

