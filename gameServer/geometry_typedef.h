#pragma once
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>

#include <boost/geometry/index/rtree.hpp>

// to store queries results
#include <vector>

#include "chrono_typedef.h"

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 3, bg::cs::cartesian> point_t;
typedef bg::model::linestring<point_t> linestring_t;
typedef bg::model::box<point_t> box;
typedef std::pair<box, unsigned> value;

typedef std::pair<time_point_t, point_t> timed_point_t;