#pragma once
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>

#include <boost/geometry/index/rtree.hpp>

// to store queries results
#include <vector>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 3, bg::cs::cartesian> point_t;
typedef bg::model::linestring<point_t> linestring_t;
typedef bg::model::box<point_t> box;
typedef std::pair<box, unsigned> value;