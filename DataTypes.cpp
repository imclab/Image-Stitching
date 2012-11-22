#include <cv.h>
#include "DataTypes.h"
#include "gpc.h"
#include "GeoReference.h"

cv::Point2f Pixel::toPoint2f(){
  return cv::Point2f(x,y);
}

gpc_vertex LatLon::toGPCVertex(){
  gpc_vertex vertex;
  vertex.x = lat;
  vertex.y = lon;
  return vertex;
}

LatLon::LatLon(gpc_vertex vertex){
  lat = vertex.x;
  lon = vertex.y;
}

cv::Point2i LatLon::toPoint2i(){
  return cv::Point2i(lat * 1000,lon*1000);
}

GPSExtremes::GPSExtremes(gpc_polygon* polygon){
    gpc_vertex* vertices = polygon->contour->vertex;
	
	minLat = INT_MAX;
	minLon = INT_MAX;
	maxLat = INT_MIN;
	maxLon = INT_MIN;

	for(int i = 0; i < 4; i++){
		if (vertices[i].y < minLon ) minLon = vertices[i].y;
		if (vertices[i].y > maxLon ) maxLon = vertices[i].y;
		if (vertices[i].x < minLat ) minLat = vertices[i].x;
	    if (vertices[i].x > maxLat ) maxLat = vertices[i].x;
	}
}

//TODO: GeoReferencing
Pixel ImageWithPlaneData::getPixelFor(LatLon latlon){
  return Pixel(0,0);
}

cv::KeyPoint Pixel::toKeyPoint(double scale){
  return cv::KeyPoint((float)x/(float)scale,(float)y/(float)scale,1.0);
}

gpc_polygon* ImageWithPlaneData::toGPCPolygon(){
  double latitude,longitude,altitude; 

  Vision::GeoReference::forwardGeoreferencing(latitude,
                                      longitude,
                                      altitude,
                                      roll,
                                      pitch,
                                      yaw,
                                      gimbalRoll,
                                      gimbalPitch,
                                      0, // gimbal yaw,
                                      0, // targetX
                                      0, // targetY
                                      1.0, // zoom,
                                      latitude,
                                      longitude,
                                      altitude);
  LatLon bottomLeftPoint = LatLon(latitude,longitude);

  Vision::GeoReference::forwardGeoreferencing(latitude,
                                      longitude,
                                      altitude,
                                      roll,
                                      pitch,
                                      yaw,
                                      gimbalRoll,
                                      gimbalPitch,
                                      0, // gimbal yaw,
                                      image.cols-1, // targetX
                                      0, // targetY
                                      1.0, // zoom,
                                      latitude,
                                      longitude,
                                      altitude);
  LatLon bottomRightPoint = LatLon(latitude,longitude);

  Vision::GeoReference::forwardGeoreferencing(latitude,
                                      longitude,
                                      altitude,
                                      roll,
                                      pitch,
                                      yaw,
                                      gimbalRoll,
                                      gimbalPitch,
                                      0, // gimbal yaw,
                                      image.cols-1, // targetX
                                      image.rows-1, // targetY
                                      1.0, // zoom,
                                      latitude,
                                      longitude,
                                      altitude);
  LatLon topRightPoint = LatLon(latitude,longitude);

  Vision::GeoReference::forwardGeoreferencing(latitude,
                                      longitude,
                                      altitude,
                                      roll,
                                      pitch,
                                      yaw,
                                      gimbalRoll,
                                      gimbalPitch,
                                      0, // gimbal yaw,
                                      0, // targetX
                                      image.rows-1, // targetY
                                      1.0, // zoom,
                                      latitude,
                                      longitude,
                                      altitude);
  LatLon topLeftPoint = LatLon(latitude,longitude);

  gpc_vertex topLeftVertex = topLeftPoint.toGPCVertex();
  gpc_vertex topRightVertex = topRightPoint.toGPCVertex();
  gpc_vertex bottomRightVertex = bottomRightPoint.toGPCVertex();
  gpc_vertex bottomLeftVertex = bottomLeftPoint.toGPCVertex();

  gpc_vertex* vertices = (gpc_vertex*)malloc(sizeof(gpc_vertex) * 4);

  vertices[0] = topLeftVertex;
  vertices[1] = topRightVertex;
  vertices[2] = bottomRightVertex;
  vertices[3] = bottomLeftVertex;

  gpc_vertex_list* list = new gpc_vertex_list();
  list->num_vertices=4;
  list->vertex = vertices;
  gpc_polygon* polygon = new gpc_polygon();
  polygon->num_contours = 1;
  polygon->hole=0;
  polygon->contour = list;

  return polygon;
}
