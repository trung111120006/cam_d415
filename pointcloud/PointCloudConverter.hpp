#pragma once
#include <librealsense2/rs.hpp>
#include "PointCloud.hpp"

struct ConvertConfig {
    float min_depth = 0.1f;   // Mét - bỏ điểm quá gần
    float max_depth = 10.0f;  // Mét - bỏ điểm quá xa
};

class PointCloudConverter {
public:
    explicit PointCloudConverter(const rs2_intrinsics& intrinsics,
                                 const ConvertConfig& config = ConvertConfig{});

    // Nhận depth frame, trả về PointCloud
    PointCloud convert(const rs2::depth_frame& depth_frame, int frame_index = 0);

private:
    rs2_intrinsics  m_intrinsics;
    ConvertConfig   m_config;
};