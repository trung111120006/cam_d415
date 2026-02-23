#include "PointCloudConverter.hpp"
#include <iostream>

using namespace std;

PointCloudConverter::PointCloudConverter(const rs2_intrinsics& intrinsics,
                                         const ConvertConfig& config)
    : m_intrinsics(intrinsics)
    , m_config(config) {
}

PointCloud PointCloudConverter::convert(const rs2::depth_frame& depth_frame,
                                         int frame_index) {
    PointCloud cloud;
    cloud.frame_index = frame_index;
    cloud.points.reserve(m_intrinsics.width * m_intrinsics.height);

    for (int v = 0; v < m_intrinsics.height; v++) {
        for (int u = 0; u < m_intrinsics.width; u++) {

            float Z = depth_frame.get_distance(u, v);

            // Lọc điểm không hợp lệ
            if (Z < m_config.min_depth || Z > m_config.max_depth)
                continue;

            // -----------------------------------------------
            // Công thức Pinhole Camera Model (tự implement):
            //   X = (u - cx) / fx * Z
            //   Y = (v - cy) / fy * Z
            //   Z = depth value (mét)
            // -----------------------------------------------
            Point3D p;
            p.x = (u - m_intrinsics.ppx) / m_intrinsics.fx * Z;
            p.y = (v - m_intrinsics.ppy) / m_intrinsics.fy * Z;
            p.z = Z;

            cloud.points.push_back(p);
        }
    }

    cout << "[Converter] Frame #" << frame_index
         << " -> " << cloud.points.size() << " points" << endl;

    return cloud;
}