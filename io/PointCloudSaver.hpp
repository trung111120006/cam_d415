#pragma once
#include "../pointcloud/PointCloud.hpp"
#include <string>

enum class SaveFormat {
    PLY,
    PCD
};

class PointCloudSaver {
public:
    explicit PointCloudSaver(const string& output_dir = ".",
                             SaveFormat format = SaveFormat::PLY);

    // Lưu cloud, tự đặt tên file theo saved_count
    bool save(const PointCloud& cloud);

    int getSavedCount() const { return m_saved_count; }

private:
    bool savePLY(const PointCloud& cloud, const string& filepath);
    bool savePCD(const PointCloud& cloud, const string& filepath);
    string buildFilePath(int index) const;

    string      m_output_dir;
    SaveFormat  m_format;
    int         m_saved_count = 0;
};