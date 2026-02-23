#pragma once
#include <librealsense2/rs.hpp>

// Chứa các thông số cấu hình camera
struct CameraConfig {
    int width  = 848;
    int height = 480;
    int fps    = 60;
};

class RealSenseCamera {
public:
    explicit RealSenseCamera(const CameraConfig& config);
    ~RealSenseCamera();

    void        start();
    void        stop();
    bool        isRunning() const;

    // Lấy 1 depth frame mới nhất từ camera
    rs2::depth_frame    getDepthFrame();

    // Lấy intrinsics để dùng khi convert point cloud
    rs2_intrinsics      getIntrinsics() const;

private:
    void warmUp(int frames = 30);  // Bỏ vài frame đầu cho camera ổn định

    CameraConfig        m_config;
    rs2::pipeline       m_pipeline;
    rs2::config         m_rs_config;
    rs2_intrinsics      m_intrinsics;
    bool                m_running = false;
};