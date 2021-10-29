/*
 * Submap.hpp
 *
 *  Created on: Oct 27, 2021
 *      Author: jelavice
 */


#pragma once

#include <open3d/geometry/PointCloud.h>
#include <ros/time.h>
#include <Eigen/Dense>
#include "m545_volumetric_mapping/Parameters.hpp"
#include "m545_volumetric_mapping/croppers.hpp"
#include "m545_volumetric_mapping/time.hpp"


namespace m545_mapping {

class Submap {

public:
	using PointCloud = open3d::geometry::PointCloud;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	Submap();
	~Submap() = default;

	void setParameters(const MapperParameters &mapperParams);
	bool insertScan(const PointCloud &rawScan,const PointCloud &preProcessedScan, const Eigen::Isometry3d &transform);
	void voxelizeInsideCroppingVolume(const CroppingVolume &cropper, const MapBuilderParameters &param, PointCloud *map) const;
	const Eigen::Isometry3d &getMapToSubmap() const;
	const PointCloud &getMap() const;
	const PointCloud& getDenseMap() const;
	void setMapToSubmap(const Eigen::Isometry3d &T);
	bool isEmpty() const;


	mutable PointCloud toRemove_;
	mutable PointCloud scanRef_;
	mutable PointCloud mapRef_;

private:
	void update(const MapperParameters &mapperParams);
	void estimateNormalsIfNeeded(PointCloud *pcl) const;
	void carve(const PointCloud &rawScan, const Eigen::Isometry3d &mapToRangeSensor, const CroppingVolume &cropper, const SpaceCarvingParameters &params,
			PointCloud *map, Timer *timer) const;

	PointCloud map_;
	PointCloud denseMap_;
	Eigen::Isometry3d mapToSubmap_ = Eigen::Isometry3d::Identity();
	Eigen::Isometry3d mapToRangeSensor_ = Eigen::Isometry3d::Identity();
	std::shared_ptr<CroppingVolume> mapBuilderCropper_;
	std::shared_ptr<CroppingVolume> denseMapCropper_;
	MapperParameters params_;
	Timer carvingTimer_;
	Timer carveDenseMapTimer_;
};



class SubmapCollection{
public:
	using Submaps = std::vector<Submap>;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	SubmapCollection();
	~SubmapCollection() = default;

	using PointCloud = open3d::geometry::PointCloud;
	void setMapToRangeSensor(const Eigen::Isometry3d &T);
	const Submap &getActiveSubmap() const;
	bool insertScan(const PointCloud &rawScan,const PointCloud &preProcessedScan, const Eigen::Isometry3d &mapToRangeSensor);
	void setParameters(const MapperParameters &p);
	bool isEmpty() const;
	const Submaps &getSubmaps() const;
	size_t getTotalNumPoints() const;
private:

	void updateActiveSubmap();
	void createNewSubmap(const Eigen::Isometry3d &mapToSubmap);
	size_t findClosestSubmap(const Eigen::Isometry3d &mapToRangesensor) const;


	Eigen::Isometry3d mapToRangeSensor_ = Eigen::Isometry3d::Identity();
	std::vector<Submap> submaps_;
	size_t activeSubmapIdx_ = 0;
	MapperParameters params_;
	size_t numScansMergedInActiveSubmap_=0;

};

} // namespace m545_mapping