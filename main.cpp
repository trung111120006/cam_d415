#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include "camera/RealSenseCamera.hpp"
#include "pointcloud/PointCloudConverter.hpp"
#include "io/PointCloudSaver.hpp"

using namespace std;

static bool g_running = true;
void signalHandler(int) { g_running = false; }

int main() {
    signal(SIGINT, signalHandler);

    CameraConfig cam_cfg;
    cam_cfg.width         = 848;
    cam_cfg.height        = 480;
    cam_cfg.fps           = 60;
    cam_cfg.warmup_frames = 30;
    cam_cfg.timeout_ms    = 5000;  // 5 giây timeout mỗi frame

    ConvertConfig conv_cfg;
    conv_cfg.min_depth = 0.1f;
    conv_cfg.max_depth = 10.0f;

    const int SAVE_EVERY = 5;

    RealSenseCamera camera(cam_cfg);

    try {
        camera.start();
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        cerr << "[Main] Tip: Kiểm tra cáp USB 3.0, thử giảm fps xuống 30." << endl;
        return -1;
    }

    PointCloudConverter converter(camera.getIntrinsics(), conv_cfg);
    PointCloudSaver     saver(".", SaveFormat::PLY);

    int frame_count   = 0;
    int missed_frames = 0;

    cout << "\n[Main] Running. Press Ctrl+C to stop.\n" << endl;

    while (g_running) {
        rs2::depth_frame depth = camera.getDepthFrame();

        if (!depth) {
            // Poll chưa có frame — sleep nhẹ tránh CPU 100%
            missed_frames++;
            this_thread::sleep_for(chrono::milliseconds(1));
            continue;
        }

        frame_count++;

        if (frame_count % SAVE_EVERY == 0) {
            PointCloud cloud = converter.convert(depth, frame_count);
            saver.save(cloud);
        }

        // Log tình trạng mỗi 60 frame
        if (frame_count % 60 == 0) {
            cout << "[Main] Frames: " << frame_count
                 << " | Saved: " << saver.getSavedCount()
                 << " | Missed polls: " << missed_frames << endl;
        }
    }

    cout << "\n[Main] Done. Frames: " << frame_count
         << " | Saved: " << saver.getSavedCount() << endl;

    camera.stop();
    return 0;
}