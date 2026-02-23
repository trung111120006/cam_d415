#include "RealSenseCamera.hpp"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

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

        warmUp();
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
    try {
        // Dùng timeout_ms thay vì block mãi mãi
        rs2::frameset frames;
        if (!m_pipeline.poll_for_frames(&frames)) {
            // poll không block — nếu chưa có frame thì trả về empty
            return rs2::depth_frame(rs2::frame{});
        }
        return frames.get_depth_frame();
    }
    catch (const rs2::error& e) {
        cerr << "[Camera] getDepthFrame error: " << e.what() << endl;
        return rs2::depth_frame(rs2::frame{});
    }
}

rs2_intrinsics RealSenseCamera::getIntrinsics() const {
    return m_intrinsics;
}

// --- private ---

void RealSenseCamera::warmUp() {
    cout << "[Camera] Warming up (" << m_config.warmup_frames << " frames)..." << endl;

    int success = 0;
    int attempts = 0;
    const int max_attempts = m_config.warmup_frames * 3; // Thử tối đa gấp 3 lần

    while (success < m_config.warmup_frames && attempts < max_attempts) {
        attempts++;
        try {
            // Dùng wait_for_frames với timeout tự đặt
            rs2::frameset fs = m_pipeline.wait_for_frames(m_config.timeout_ms);
            if (fs.get_depth_frame()) {
                success++;
            }
        }
        catch (const rs2::error& e) {
            // Frame drop trong warm-up — không crash, chỉ log và thử lại
            cerr << "[Camera] Warm-up frame drop (" << attempts << "): "
                 << e.what() << endl;

            // Chờ 100ms rồi thử lại
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }

    if (success < m_config.warmup_frames) {
        cerr << "[Camera] Warning: Only got " << success << "/"
             << m_config.warmup_frames << " warm-up frames." << endl;
    } else {
        cout << "[Camera] Ready! (" << attempts << " attempts)" << endl;
    }
}