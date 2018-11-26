/**
 *
 *
 */

#pragma once

/* #include "cinder/TriMesh.h" */
#include "cinder/gl/VboMesh.h"
#include "cinder/Color.h"
#include <igl/igl_inline.h>
#include <igl/viewer/ViewerData.h>
#include <igl/per_vertex_normals.h>
#include <igl/per_face_normals.h>
#include <igl/per_corner_normals.h>
#include <igl/AABB.h>

namespace cinder {
typedef std::shared_ptr<class IglMesh> IglMeshRef;
class IglMesh : public VboMesh{
  using VboMesh::VboMesh;
 public:
  IglMesh();
  IglMesh(const std::string str);
  IglMesh(Eigen::MatrixXd V, Eigen::MatrixXi F, Eigen::MatrixXd C=Eigen::MatrixXd()) {
    IglMesh();
    setMesh(V, F, C);
  };
  virtual ~IglMesh();
  bool loadMesh(const std::string str);
  void setMesh(Eigen::MatrixXd V, Eigen::MatrixXi F,Eigen::MatrixXd C=Eigen::MatrixXd());
  void tetrahedralize();
  void setV(Eigen::MatrixXd V);
  Eigen::MatrixXd getV() { return mV; };
  Eigen::MatrixXi getF() { return mF; };
  Eigen::MatrixXd* getVRef() { return &mV; };
  Eigen::MatrixXi* getFRef() { return &mF; };
  void setColor(Eigen::MatrixXd C);
  igl::AABB<Eigen::MatrixXd, 3> getAABBTree() { return aabbTree; };

 public:
  std::vector<gl::VboMesh::Layout()>
  Eigen::MatrixXd mTV;
  Eigen::MatrixXi mTT;
  Eigen::MatrixXi mTF;
  Eigen::MatrixXd mV;
  Eigen::MatrixXd mC;
  Eigen::MatrixXi mF;
  Eigen::MatrixXi mE;

 private:
  igl::AABB<Eigen::MatrixXd, 3> aabbTree;

  /* TriMesh::Format fmt; */

  Eigen::MatrixXd N_vertices;
  Eigen::MatrixXd N_faces;
  Eigen::MatrixXd N_corners;
  /* Eigen::MatrixXf V_vbo; */
  /* Eigen::MatrixXf V_normals_vbo; */
  /* Eigen::MatrixXf V_ambient_vbo; */
  /* Eigen::MatrixXf V_diffuse_vbo; */
  /* Eigen::MatrixXf V_specular_vbo; */
  /* Eigen::MatrixXf V_uv_vbo; */
  /* Eigen::MatrixXf lines_V_vbo; */
  /* Eigen::MatrixXf lines_V_colors_vbo; */
  /* Eigen::MatrixXf points_V_vbo; */
  /* Eigen::MatrixXf points_V_colors_vbo; */

  /* int tex_u; */
  /* int tex_v; */
  /* Eigen::Matrix<char, Eigen::Dynamic, 1> tex; */

  /* Eigen::Matrix<unsigned, Eigen::Dynamic, Eigen::Dynamic> F_vbo; */
  /* Eigen::Matrix<unsigned, Eigen::Dynamic, Eigen::Dynamic> lines_F_vbo; */
  /* Eigen::Matrix<unsigned, Eigen::Dynamic, Eigen::Dynamic> points_F_vbo; */

  /* Eigen::MatrixXf C_vbo; */
};
}
