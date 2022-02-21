/*
 * output.cpp
 *
 *  Created on: Feb 21, 2022
 *      Author: jelavice
 */




#include "open3d_slam/output.hpp"
#include "open3d_slam/math.hpp"

#include <memory>
#include <Eigen/Dense>
#include <open3d/io/PointCloudIO.h>

namespace o3d_slam {

std::string asString(const Transform &T) {
	const double kRadToDeg = 180.0 / M_PI;
	const auto &t = T.translation();
	const auto &q = Eigen::Quaterniond(T.rotation());
	const std::string trans = string_format("t:[%f, %f, %f]", t.x(), t.y(), t.z());
	const std::string rot = string_format("q:[%f, %f, %f, %f]", q.x(), q.y(), q.z(), q.w());
	const auto rpy = toRPY(q) * kRadToDeg;
	const std::string rpyString = string_format("rpy (deg):[%f, %f, %f]", rpy.x(), rpy.y(), rpy.z());
	return trans + " ; " + rot + " ; " + rpyString;

}

void saveToFile(const std::string &filename, const PointCloud &cloud) {
	PointCloud copy = cloud;
	std::string nameWithCorrectSuffix = filename;
	size_t found = filename.find(".pcd");
	if (found == std::string::npos) {
		nameWithCorrectSuffix = filename + ".pcd";
	}
	open3d::io::WritePointCloudToPCD(nameWithCorrectSuffix, copy, open3d::io::WritePointCloudOption());
}

} // namespace o3d_slam
