#include "CinderLibIgl.h"
#include "cinder/app/App.h"
#include <igl/read_triangle_mesh.h>
#include <igl/copyleft/tetgen/tetrahedralize.h>
#include <igl/barycenter.h>
#include <igl/edges.h>

using namespace std;
using namespace ci;
using namespace ci::app;

namespace cinder {
IglMesh::IglMesh() {
  // fmt = TriMesh::Format().positions().normals();
  // initFromFormat(fmt);
}

IglMesh::IglMesh(const std::string str) {
  fmt = TriMesh::Format().positions().normals();
  initFromFormat(fmt);
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  if (igl::read_triangle_mesh(str, V, F)) {
    setMesh(V, F);
  } else {
    cout << "error reading mesh!"
         << "\n";
  }
}

IglMesh::~IglMesh() {}

bool IglMesh::loadMesh(const std::string str) {
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  if (igl::read_triangle_mesh(str, V, F)) {
    setMesh(V, F);
    return true;
  } else {
    cout << "error reading mesh!"
         << "\n";
    return false;
  }
}

  void IglMesh::setV(Eigen::MatrixXd V) {
    mV=V;
    data.set_vertices(V);
    this->mPositions=std::vector<float>(V.data(),V.data()+V.size());
  }

  void IglMesh::setColor(Eigen::MatrixXd C) {
    if(C.size()==0){
      fmt = TriMesh::Format().positions().normals();
      initFromFormat(fmt);
    }
    else{
      fmt = TriMesh::Format().positions().normals().colors();
      initFromFormat(fmt);
    }
    data.set_colors(C);
    Eigen::MatrixXf C_vbo= C.transpose().cast<float>();
    this->mColors=std::vector<float>(C_vbo.data(),C_vbo.data()+C_vbo.size());
  };
  void IglMesh::setMesh(Eigen::MatrixXd V, Eigen::MatrixXi F,Eigen::MatrixXd C) {
  this->clear();
  mV=V;
  mF=F;
  mC=C;
  igl::edges(mF, mE);
  // cout << "edges="<<mE << "\n"; 

  aabbTree.init(mV,mF);
  // Compute per-face normals
  igl::per_face_normals(mV,mF,N_faces);

  // Compute per-vertex normals
  igl::per_vertex_normals(mV,mF,N_vertices);

  // Compute per-corner normals, |dihedral angle| > 20 degrees --> crease
  igl::per_corner_normals(mV,mF,20,N_corners);
  // cout << "N_vertices="<<N_vertices << "\n"; 
  // cout << "N_faces="<<N_faces<< "\n"; 
  // cout << "N_corners="<<N_corners<< "\n"; 
  // data.clear();
  // data.set_mesh(V, F);
  // data.set_normals(N_faces);
  // data.set_normals(N_corners);
  Eigen::MatrixXf V_vbo= V.transpose().cast<float>();
  Eigen::Matrix<unsigned, Eigen::Dynamic, Eigen::Dynamic> F_vbo= F.transpose().cast<unsigned>();
  Eigen::MatrixXf C_vbo= C.transpose().cast<float>();
  // Eigen::MatrixXf C_vbo= Eigen::MatrixXf::Zero(V.cols(),V.rows())+Eigen::MatrixXf::Ones(V.cols(),V.rows());
  // Eigen::MatrixXf C_vbo= Eigen::MatrixXf::Random(V.cols(),V.rows())+Eigen::MatrixXf::Ones(V.cols(),V.rows());
  Eigen::MatrixXf V_normals_vbo= N_vertices.transpose().cast<float>();
  // V_normals_vbo= N_faces.transpose().cast<float>();
  // V_normals_vbo = N_corners.transpose().cast<float>()
  // V_normals_vbo = (data.F_normals.transpose()).cast<float>();
  this->mPositions=std::vector<float>(V_vbo.data(),V_vbo.data()+V_vbo.size());
  this->mIndices=std::vector<uint32_t>(F_vbo.data(),F_vbo.data()+F_vbo.size());

  this->mNormals.clear();
  // (glm::vec3*)V_normals_vbo,V_normals_vbo.data()+V_normals_vbo.size()
  // this->mNormals=std::vector<vec3>(V_normals_vbo.data(),V_normals_vbo.data()+V_normals_vbo.size());
  // this->mColors=std::vector<float>(C_vbo.data(),C_vbo.data()+C_vbo.size());
  this->appendNormals((glm::vec3*)Eigen::MatrixXf(N_vertices.transpose().cast<float>()).data(), N_vertices.rows());
  // this->appendColors((Color*)(Eigen::MatrixXf(mC.transpose().cast<float>())).data(),mC.rows());
}

  void IglMesh::tetrahedralize() {
    igl::copyleft::tetgen::tetrahedralize(mV,mF,"pq1.414",mTV,mTT,mTF);
  }
}
