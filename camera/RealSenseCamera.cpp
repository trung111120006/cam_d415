#include "RealSenseCamera.hpp"
#include <iostream>
#include <stdexcept>

using namespace std;

RealSenseCamera::RealSenseCamera(const CameraConfig& config)
    : m_config(config) {
    m_rs_config.enable_stream(
        RS2_STREAM_DEPTH,
        m_config.width,
        m_config.height,
        RS2_FORMAT_Z16,
        m_config.fps
    );
}

RealSenseCamera::~RealSenseCamera() {
    if (m_running) stop();
}

void RealSenseCamera::start() {
    try {
        auto profile = m_pipeline.start(m_rs_config);

        // Lấy intrinsics ngay lúc start để dùng sau
        m_intrinsics = profile
            .get_stream(RS2_STREAM_DEPTH)
            .as<rs2::video_stream_profile>()
            .get_intrinsics();

        m_running = true;

        cout << "[Camera] Started " 
             << m_config.width << "x" << m_config.height 
             << " @ " << m_config.fps << "fps" << endl;
        cout << "[Camera] fx=" << m_intrinsics.fx 
             << " fy=" << m_intrinsics.fy
             << " cx=" << m_intrinsics.ppx 
             << " cy=" << m_intrinsics.ppy << endl;

        warmUp(30);
    }
    catch (const rs2::error& e) {
        throw runtime_error(string("[Camera] RealSense error: ") + e.what());
    }
}

void RealSenseCamera::stop() {
    if (m_running) {
        m_pipeline.stop();
        m_running = false;
        cout << "[Camera] Stopped." << endl;
    }
}

bool RealSenseCamera::isRunning() const {
    return m_running;
}

rs2::depth_frame RealSenseCamera::getDepthFrame() {
    rs2::frameset frames = m_pipeline.wait_for_frames();
    return frames.get_depth_frame();
}

rs2_intrinsics RealSenseCamera::getIntrinsics() const {
    return m_intrinsics;
}

// --- private ---

void RealSenseCamera::warmUp(int frames) {
    cout << "[Camera] Warming up (" << frames << " frames)..." << endl;
    for (int i = 0; i < frames; i++) {
        m_pipeline.wait_for_frames();
    }
    cout << "[Camera] Ready!" << endl;
}