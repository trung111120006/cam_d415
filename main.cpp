#include <iostream>
#include <csignal>
#include "camera/RealSenseCamera.hpp"
#include "pointcloud/PointCloudConverter.hpp"
#include "io/PointCloudSaver.hpp"

using namespace std;

// Cho phép dừng bằng Ctrl+C
static bool g_running = true;
void signalHandler(int) { g_running = false; }

int main() {
    signal(SIGINT, signalHandler);

    // --- Cấu hình ---
    CameraConfig    cam_cfg   { 848, 480, 60 };
    ConvertConfig   conv_cfg  { 0.1f, 10.0f };
    const int       SAVE_EVERY = 5;   // Cứ 5 frame lưu 1 lần

    // --- Khởi tạo các module ---
    RealSenseCamera camera(cam_cfg);
    camera.start();

    PointCloudConverter converter(camera.getIntrinsics(), conv_cfg);
    PointCloudSaver     saver(".", SaveFormat::PLY);

    // --- Main loop ---
    int frame_count = 0;
    cout << "\n[Main] Running. Press Ctrl+C to stop.\n" << endl;

    while (g_running) {
        rs2::depth_frame depth = camera.getDepthFrame();
        if (!depth) continue;

        frame_count++;

        // Mỗi SAVE_EVERY frame thì convert và lưu
        if (frame_count % SAVE_EVERY == 0) {
            PointCloud cloud = converter.convert(depth, frame_count);
            saver.save(cloud);
        }
    }

    cout << "\n[Main] Done. Total frames: " << frame_count
         << " | Saved: " << saver.getSavedCount() << endl;

    camera.stop();
    return 0;
}