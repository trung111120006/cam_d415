#pragma once
#include <librealsense2/rs.hpp>

struct CameraConfig {
    int width      = 848;
    int height     = 480;
    int fps        = 60;
    int warmup_frames   = 30;
    unsigned int timeout_ms = 5000;  // Timeout mỗi lần wait_for_frames
};

class RealSenseCamera {
public:
    explicit RealSenseCamera(const CameraConfig& config);
    ~RealSenseCamera();

    void             start();
    void             stop();
    bool             isRunning() const;
    rs2::depth_frame getDepthFrame();
    rs2_intrinsics   getIntrinsics() const;

private:
    void warmUp();

    CameraConfig   m_config;
    rs2::pipeline  m_pipeline;
    rs2::config    m_rs_config;
    rs2_intrinsics m_intrinsics;
    bool           m_running = false;
};