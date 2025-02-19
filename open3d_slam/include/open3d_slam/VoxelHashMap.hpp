/*
 * VoxelHashMap.hpp
 *
 *  Created on: Mar 30, 2022
 *      Author: jelavice
 */

/*
 * Voxel.hpp
 *
 *  Created on: Oct 19, 2021
 *      Author: jelavice
 */

#pragma once

#include <Eigen/Core>
#include <vector>
#include <unordered_map>
#include <map>
#include <open3d_slam/typedefs.hpp>

namespace o3d_slam {

struct EigenVec3iHash {
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  static constexpr size_t sl = 17191;
  static constexpr size_t sl2 = sl * sl;

  std::size_t operator()(const Eigen::Vector3i& index) const {
    return static_cast<unsigned int>(index.x() + index.y() * sl +
                                     index.z() * sl2);
  }
};

inline Eigen::Vector3i getVoxelIdx(const Eigen::Vector3d &p, const Eigen::Vector3d &voxelSize) {
	Eigen::Vector3d coord = p.array() / voxelSize.array();
	return Eigen::Vector3i(int(std::floor(coord(0))), int(std::floor(coord(1))), int(std::floor(coord(2))));
}

inline Eigen::Vector3i getVoxelIdx(const Eigen::Vector3d &p, const Eigen::Vector3d &voxelSize,
		const Eigen::Vector3d &minBound) {
	Eigen::Vector3d coord = (p - minBound).array() / voxelSize.array();
	return Eigen::Vector3i(int(std::floor(coord(0))), int(std::floor(coord(1))), int(std::floor(coord(2))));
}

inline std::pair<Eigen::Vector3d, Eigen::Vector3d> computeVoxelBounds(const open3d::geometry::PointCloud &cloud,
		const Eigen::Vector3d &voxelSize) {
	const Eigen::Vector3d voxelMinBound = cloud.GetMinBound() - voxelSize * 0.5;
	const Eigen::Vector3d voxelMaxBound = cloud.GetMaxBound() + voxelSize * 0.5;
	return {voxelMinBound, voxelMaxBound};
}

template<typename Voxel>
class VoxelHashMap {
public:
EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	using Voxel_t = Voxel;
	VoxelHashMap() :
			VoxelHashMap(Eigen::Vector3d::Constant(0.25)) {
	}
	VoxelHashMap(const Eigen::Vector3d &voxelSize) :
			voxelSize_(voxelSize) {
	}
	bool hasVoxelContainingPoint(const Eigen::Vector3d &p) const {
		const auto voxelIdx = getVoxelIdx(p, voxelSize_);
		const auto search = voxels_.find(voxelIdx);
		return search != voxels_.end();
	}
	bool hasVoxelWithKey(const Eigen::Vector3i &key) const {
		const auto search = voxels_.find(key);
		return search != voxels_.end();
	}
	size_t size() const {
		return voxels_.size();
	}
	void clear() {
		voxels_.clear();
	}
	bool empty() const {
		return voxels_.empty();
	}
	Eigen::Vector3i getKey(const Eigen::Vector3d &p) const {
		return getVoxelIdx(p, voxelSize_);
	}
	void removeKey(const Eigen::Vector3i &k) {
		voxels_.erase(k);
	}

	Voxel *getVoxelPtr(const Eigen::Vector3i &key) {
		auto search = voxels_.find(key);
		return search != voxels_.end() ? &(search->second) : nullptr;
	}

	Voxel *getVoxelContainingPointPtr(const Eigen::Vector3d &p) {
		const Eigen::Vector3i key = getVoxelIdx(p, voxelSize_);
		auto search = voxels_.find(key);
		return search != voxels_.end() ? &(search->second) : nullptr;
	}

	const Voxel *getVoxelPtr(const Eigen::Vector3i &key) const {
		const auto search = voxels_.find(key);
		return search != voxels_.end() ? &(search->second) : nullptr;
	}

	const Voxel *getVoxelContainingPointPtr(const Eigen::Vector3d &p) const {
		const Eigen::Vector3i key = getVoxelIdx(p, voxelSize_);
		const auto search = voxels_.find(key);
		return search != voxels_.end() ? &(search->second) : nullptr;
	}

	Eigen::Vector3d getVoxelSize() const {
		return voxelSize_;
	}

	std::unordered_map<Eigen::Vector3i, Voxel, EigenVec3iHash> voxels_;
protected:
	Eigen::Vector3d voxelSize_;

};


} // namespace o3d_slam

