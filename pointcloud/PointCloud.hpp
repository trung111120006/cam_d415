// File pointclound này chịu trách nhiệm lưu trữ dữ liệu point cloud thu được từ camera depth hoặc LiDAR. Nó bao gồm một vector các điểm 3D, chỉ số frame và timestamp (nếu cần). PointCloud có thể được sử dụng để lưu trữ và xử lý dữ liệu point cloud trong các ứng dụng như SLAM, mapping, hoặc object detection.
#pragma once
#include <vector>
#include <string>
 
using namespace std;

struct Point3D {
    float x, y, z;
};

struct PointCloud {
    vector<Point3D> points;
    int frame_index = 0;      // Frame số mấy tạo ra cloud này
    string timestamp;    // Thời điểm capture (tuỳ chọn)
};