#ifndef LIBXENOVERSE_EMD_H_INCLUDED
#define LIBXENOVERSE_EMD_H_INCLUDED

#define LIBXENOVERSE_EMD_SIGNATURE           "#EMD"
#define LIBXENOVERSE_EMD_SUBMESH_BONE_LIMIT  24

namespace LibXenoverse {
	class EMDVertex {
		public:
			float x, y, z;
			float nx, ny, nz;
			float u, v;
			unsigned char bone[4];
			float bone_weight[4];

			EMDVertex() {
			}

			void zero() {
				x = y = z = nx = ny = nz = u = v = 0.0f;
				bone[0] = bone[1] = bone[2] = bone[3] = 0;
				bone_weight[0] = 1.0f;
				bone_weight[1] = bone_weight[2] = bone_weight[3] = 0.0f;
			}

			bool operator == (const EMDVertex& vertex) {
				if (x != vertex.x) return false;
				if (y != vertex.y) return false;
				if (z != vertex.z) return false;
				if (nx != vertex.nx) return false;
				if (ny != vertex.ny) return false;
				if (nz != vertex.nz) return false;
				if (u != vertex.u) return false;
				if (v != vertex.v) return false;

				for (size_t i = 0; i<4; i++) {
					if (bone[i] != vertex.bone[i]) return false;
					if (bone_weight[i] != vertex.bone_weight[i]) return false;
				}

				return true;
			}

			void read(File *file, unsigned int vertex_type_flag);
			void write(File *file, unsigned int vertex_type_flag);

	};

	class EMDTriangles {
		public:
			vector<unsigned short> faces;
			vector<string> bone_names;

			EMDTriangles() {
			}

			void read(File *file);
			void write(File *file);
	};

	class EMDSubmeshDefinition {
		public:
			unsigned int flag;
			float a;
			float b;

			EMDSubmeshDefinition() {
			}

			void read(File *file);
			void write(File *file);
	};

	class EMDSubmesh {
		protected:
			vector<EMDVertex> vertices;
			vector<EMDTriangles> triangles;
			vector<EMDSubmeshDefinition> definitions;
			float float_group[12];

			unsigned int vertex_type_flag;
			unsigned int vertex_size;

			string name;
		public:
			EMDSubmesh() {
			}

			void read(File *file);
			void write(File *file);

			void setVertexScale(float scale);

			void getTotalData(list<EMDVertex *> *vertex_list, list<unsigned int> *face_list,
				list<unsigned int> *submesh_vertex_counts, list<vector<string>> *submesh_bone_groups);

			/** Get the best triangle list suited for the array of bone names.
			    If there's no good triangle list that can fit all the bone names,
				create a new one and return it.

				A triangle list won't be fit for the bone names if it has reached 
				the bone limit (24), so it's necessary to create new ones depending on
				the bone count of the submesh.				
			*/
			EMDTriangles *getTriangleListFor(vector<string> bone_names);

			#ifdef LIBXENOVERSE_FBX_SUPPORT
			void importFBX(FbxMesh *lMesh, int material_index, FbxAMatrix transform_matrix, vector<string> &material_names, vector<vector<pair<double, FbxNode *>>> &control_points_skin_bindings);
			#endif
	};

	class EMDMesh {
		protected:
			vector<EMDSubmesh *> submeshes;
			float float_group[12];
			string name;
		public:
			EMDMesh() {
			}

			void read(File *file);
			void write(File *file);

			void setVertexScale(float scale);

			string getName() {
				return name;
			}
			
			void getTotalData(list<EMDVertex *> *vertex_list, list<unsigned int> *face_list,
				list<unsigned int> *submesh_vertex_counts, list<vector<string>> *submesh_bone_groups);

			#ifdef LIBXENOVERSE_FBX_SUPPORT
				void importFBX(FbxNode *lNode);
			#endif
	};

	class EMDModel {
		protected:
			vector<EMDMesh *> meshes;
		public:
			EMDModel() {
			}

			void read(File *file);
			void write(File *file);

			void setVertexScale(float scale);

			vector<string> getMeshNames() {
				vector<string> names;
				for (size_t i = 0; i < meshes.size(); i++) {
					names.push_back(meshes[i]->getName());
				}
				return names;
			}

			void getTotalData(list<EMDVertex *> *vertex_list, list<unsigned int> *face_list,
				list<unsigned int> *submesh_vertex_counts, list<vector<string>> *submesh_bone_groups);

			#ifdef LIBXENOVERSE_FBX_SUPPORT
				void importFBX(FbxNode *lNode);
			#endif
	};

	class EMD {
		protected:
			vector<EMDModel *> models;
			vector<string> model_names;
		public:
			EMD() {
			}

			EMD(string filename);
			void save(string filename, bool big_endian = false);

			void read(File *file);
			void write(File *file);

			void setVertexScale(float scale);

			#ifdef LIBXENOVERSE_FBX_SUPPORT
				FbxMesh *createFBXMesh(FbxScene *scene);
				void createFBXSkin(FbxScene *scene, FbxMesh *fbx_mesh, vector<FbxNode *> &fbx_bones, FbxAMatrix skin_matrix);
				vector<int> createFBXBoneMap(vector<string> bone_table, vector<FbxNode *> &fbx_bones);

				void importFBX(FbxNode *lNode);
			#endif

			void getTotalData(list<EMDVertex *> *vertex_list, list<unsigned int> *face_list, 
						      list<unsigned int> *submesh_vertex_counts, list<vector<string>> *submesh_bone_groups);
	};

}

#endif