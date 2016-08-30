#ifndef _SBPL_COLLISION_SPACE_
#define _SBPL_COLLISION_SPACE_

// standard includes
#include <cmath>
#include <vector>

// system includes
#include <angles/angles.h>
#include <geometry_msgs/Point.h>
#include <moveit_msgs/CollisionObject.h>
#include <moveit_msgs/PlanningScene.h>
#include <moveit_msgs/RobotState.h>
#include <ros/ros.h>
#include <sbpl_geometry_utils/bounding_spheres.h>
#include <sbpl_geometry_utils/interpolate.h>
#include <sbpl_geometry_utils/utils.h>
#include <sbpl_geometry_utils/voxelize.h>
#include <tf_conversions/tf_kdl.h>

// project includes
#include <sbpl_adaptive_collision_checking/sbpl_collision_model.h>
#include <sbpl_adaptive_collision_checking/occupancy_grid.h>
#include <sbpl_adaptive_collision_checking/bresenham.h>

namespace sbpl_adaptive_collision_checking {

class SBPLCollisionSpace
{
public:

    SBPLCollisionSpace(
        std::shared_ptr<SBPLCollisionModel> model,
        std::shared_ptr<OccupancyGrid> grid);

    ~SBPLCollisionSpace();

    void setPadding(double padding);
    void setContactPadding(double padding);

    /// \name Collision Detection
    ///@{
    bool checkCollision(const ModelCoords_t &coords, double &dist);

    bool checkCollision(
        const ModelCoords_t &coords0,
        const ModelCoords_t &coords1,
        int steps,
        double &dist);
    ///@}

    /// \name Contact Detection
    ///@{
    bool checkContact(const ModelCoords_t &coords, double &dist);

    bool checkContact(
        const ModelCoords_t &coords,
        const std::string &link_name,
        double &dist);

    bool checkContact(
        const ModelCoords_t &coords0,
        const ModelCoords_t &coords1,
        int steps,
        double &dist);
    ///@}

    double getResolution();

    std::string getReferenceFrame();

    void getSize(int &dim_x, int &dim_y, int &dim_z);

    bool getModelVoxelsInGrid(
        const ModelCoords_t &coords,
        std::vector<Eigen::Vector3i> &voxels);

    std::shared_ptr<const SBPLCollisionModel> getModelPtr();

    bool isValidPoint(double x, double y, double z) const;

private:

    std::shared_ptr<SBPLCollisionModel> model_;
    std::shared_ptr<OccupancyGrid> grid_;

    double padding_;
    double contact_padding_;

    inline bool isValidCell(
        const int x,
        const int y,
        const int z,
        const int radius);
    double isValidLineSegment(
        const std::vector<int>& a,
        const std::vector<int>& b,
        const int radius);
    bool getClearance(
        const ModelCoords_t &coords,
        int num_spheres,
        double &avg_dist,
        double &min_dist);
};

inline
double SBPLCollisionSpace::getResolution()
{
    return grid_->getResolution();
}

inline
std::string SBPLCollisionSpace::getReferenceFrame()
{
    return grid_->getReferenceFrame();
}

inline
void SBPLCollisionSpace::getSize(int &dim_x, int &dim_y, int &dim_z)
{
    grid_->getGridSize(dim_x, dim_y, dim_z);
}

inline
std::shared_ptr<const SBPLCollisionModel> SBPLCollisionSpace::getModelPtr()
{
    std::shared_ptr<const SBPLCollisionModel> ptr = model_;
    return ptr;
}

inline
bool SBPLCollisionSpace::isValidPoint(double x, double y, double z) const
{
    int gx, gy, gz;
    grid_->worldToGrid(x, y, z, gx, gy, gz);
    if (grid_->isInBounds(gx, gy, gz)) {
        if (grid_->getDistance(gx, gy, gz) > grid_->getResolution()) {
            return true;
        }
    }
    return false;
}

inline
bool SBPLCollisionSpace::isValidCell(
    const int x,
    const int y,
    const int z,
    const int radius)
{
    if (grid_->getCell(x, y, z) <= radius) {
        return false;
    }
    return true;
}

} // namespace sbpl_adaptive_collision_checking

#endif

