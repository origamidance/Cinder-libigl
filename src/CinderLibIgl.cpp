#include "CinderLibIgl.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/gl/gl.h"
#include <igl/read_triangle_mesh.h>
#include <igl/barycenter.h>
#include <igl/edges.h>


using namespace std;
using namespace ci;
using namespace ci::app;

namespace cinder {
    IglMesh::IglMesh() {
      bufferLayout = {
              gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::POSITION, 3),
              gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::NORMAL, 3),
              gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::COLOR, 3)
      };
      auto lambert = gl::ShaderDef().lambert().color();
      mShaderRef= gl::getStockShader( lambert );
    }

    IglMesh::IglMesh(const std::string str) {
      bufferLayout = {
              gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::POSITION, 3),
              gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::NORMAL, 3),
              gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::COLOR, 3)
      };
      auto lambert = gl::ShaderDef().lambert().color();
      mShaderRef= gl::getStockShader( lambert );
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

  void IglMesh::setShader(gl::GlslProgRef shader){
      mShaderRef=shader;
    mBatchRef=gl::Batch::create(mVboMeshRef,mShaderRef);
    }


    void IglMesh::setColor(Eigen::MatrixXd C) {
      Eigen::MatrixXf C_vbo = C.transpose().cast<float>();
      mVboMeshRef->bufferAttrib(geom::Attrib::COLOR, C_vbo.size() * sizeof(float), (float *) C_vbo.data());
    };

    void IglMesh::setMesh(Eigen::MatrixXd V, Eigen::MatrixXi F, Eigen::MatrixXd C) {
      mV = V;
      mF = F;
      mC = C;
      igl::edges(mF, mE);
      // Compute per-face normals
      igl::per_face_normals(mV, mF, N_faces);

      // Compute per-vertex normals
      igl::per_vertex_normals(mV, mF, N_vertices);

      // Compute per-corner normals, |dihedral angle| > 20 degrees --> crease
      igl::per_corner_normals(mV, mF, 20, N_corners);
      Eigen::MatrixXf V_vbo = V.transpose().cast<float>();
      Eigen::Matrix<unsigned, Eigen::Dynamic, Eigen::Dynamic> F_vbo = F.transpose().cast<unsigned>();
      Eigen::MatrixXf C_vbo;
      if (!C.size())
        C_vbo = Eigen::MatrixXf::Ones(V_vbo.rows(), V_vbo.cols());
      else
        C_vbo = C.transpose().cast<float>();
      Eigen::MatrixXf V_normals_vbo = N_vertices.transpose().cast<float>();

      mVboMeshRef = gl::VboMesh::create(V.rows(), GL_TRIANGLES, bufferLayout, F.size(), GL_UNSIGNED_INT);
      mVboMeshRef->bufferAttrib(geom::Attrib::POSITION, V_vbo.size() * sizeof(float), (float *) V_vbo.data());
      mVboMeshRef->bufferAttrib(geom::Attrib::NORMAL,V_normals_vbo.size()* sizeof(float),(float*) V_normals_vbo.data());
      mVboMeshRef->bufferAttrib(geom::Attrib::COLOR, C_vbo.size() * sizeof(float), (float *) C_vbo.data());
      mVboMeshRef->bufferIndices(F_vbo.size() * sizeof(uint), (uint *) (F_vbo.data()));
      mBatchRef=gl::Batch::create(mVboMeshRef,mShaderRef);
    }
    void IglMesh::draw() {
      mBatchRef->draw();
    }
}
