#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ply_utilities/ply.h"

#include "PLYLoader.hpp"

using namespace std;

#define X 0
#define Y 1
#define Z 2

typedef float Point[3];
typedef float Vector[3];

typedef struct Vertex {
	Point coord;
	Point normal;
	void *other_props;       /* other properties */
} Vertex;

typedef struct Face {
	unsigned char nverts;    /* number of vertex indices in list */
	int *verts;              /* vertex index list */
	void *other_props;       /* other properties */
} Face;

PlyProperty vert_props[] = { /* list of property information for a vertex */
	{"x", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex,coord[X]), 0, 0, 0, 0},
	{"y", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex,coord[Y]), 0, 0, 0, 0},
	{"z", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex,coord[Z]), 0, 0, 0, 0},
	{"nx", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex,normal[X]), 0, 0, 0, 0},
	{"ny", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex,normal[Y]), 0, 0, 0, 0},
	{"nz", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex,normal[Z]), 0, 0, 0, 0},
};

PlyProperty face_props[] = { /* list of property information for a vertex */
	{"vertex_indices", PLY_INT, PLY_INT, offsetof(Face,verts),
			1, PLY_UCHAR, PLY_UCHAR, offsetof(Face,nverts)},
};

static PlyOtherElems *other_elements = NULL;
static PlyOtherProp *vert_other,*face_other;
static int nelems;
static char **elist;
static int num_comments;
static char **comments;
static int num_obj_info;
static char **obj_info;
static int file_type;


int has_x, has_y, has_z;
int has_nx, has_ny, has_nz;

int has_vertex_indices;

void loadPly(const char *fileName, vector<vec>& vertices, vector<vec>& normals, vector<int3>& faces)
{
	FILE* file = fopen(fileName, "r");

	if(file)
	{
		int i,j;
		PlyFile *ply;
		int nprops;
		int num_elems;
		PlyProperty **plist;
		char *elem_name;
		float version;

		/*** Read in the original PLY object ***/

		ply  = ply_read (file, &nelems, &elist);
		ply_get_info (ply, &version, &file_type);

		for (i = 0; i < nelems; i++) {

			/* get the description of the first element */
			elem_name = elist[i];
			plist = ply_get_element_description (ply, elem_name, &num_elems, &nprops);

			if (equal_strings ("vertex", elem_name)) {

				/* set up for getting vertex elements */
				has_x = has_y = has_z = false;
				has_nx = has_ny = has_nz = false;
	      
				for (j=0; j<nprops; j++)
				{
					if (equal_strings("x", plist[j]->name))
					{
						ply_get_property (ply, elem_name, &vert_props[0]);  /* x */
						has_x = true;
					}
					else if (equal_strings("y", plist[j]->name))
					{
						ply_get_property (ply, elem_name, &vert_props[1]);  /* y */
						has_y = true;
					}
					else if (equal_strings("z", plist[j]->name))
					{
						ply_get_property (ply, elem_name, &vert_props[2]);  /* z */
						has_z = true;
					}
					else if (equal_strings("nx", plist[j]->name))
					{
						ply_get_property (ply, elem_name, &vert_props[0]);  /* nx */
						has_nx = true;
					}
					else if (equal_strings("ny", plist[j]->name))
					{
						ply_get_property (ply, elem_name, &vert_props[1]);  /* ny */
						has_ny = true;
					}
					else if (equal_strings("nz", plist[j]->name))
					{
						ply_get_property (ply, elem_name, &vert_props[2]);  /* nz */
						has_nz = true;
					}
				}
				vert_other = ply_get_other_properties (ply, elem_name,
						offsetof(Vertex,other_props));

				/* test for necessary properties */
				if ((!has_x) || (!has_y) || (!has_z))
				{
					fprintf(stderr, "Vertices don't have x, y, and z\n");
					exit(-1);
				}
	      
				/* grab all the vertex elements */
				for (j = 0; j < num_elems; j++) {
					Vertex v;
					ply_get_element (ply, (void *) &v);
					vertices.push_back(vec(v.coord[0], v.coord[1], v.coord[2]));
					if(has_nx) normals.push_back(vec(v.normal[0], v.normal[1], v.normal[2]));
				}
			}
			else if (equal_strings ("face", elem_name)) {

				/* set up for getting face elements */
				/* verify which properties these vertices have */
				has_vertex_indices = false;

				for (j=0; j<nprops; j++)
				{
					if (equal_strings("vertex_indices", plist[j]->name))
					{
						ply_get_property (ply, elem_name, &face_props[0]);  /* vertex_indices */
						has_vertex_indices = true;
					}
				}
				face_other = ply_get_other_properties (ply, elem_name,
						offsetof(Face,other_props));

				/* test for necessary properties */
				if (!has_vertex_indices)
				{
					fprintf(stderr, "Faces don't have vertex indices\n");
					exit(-1);
				}
	      
				/* grab all the face elements */
				for (j = 0; j < num_elems; j++) {
					Face f;
					ply_get_element (ply, (void *) &f);
					int3 face;
					face.data[0] = f.verts[0];
					face.data[1] = f.verts[1];
					face.data[2] = f.verts[2];
					faces.push_back(face);
				}
			}
			else
				other_elements = ply_get_other_element (ply, elem_name, num_elems);
		}

		comments = ply_get_comments (ply, &num_comments);
		obj_info = ply_get_obj_info (ply, &num_obj_info);

		ply_close (ply);
	}
}

unsigned long getFaceCountFromPly(const char *fileName)
{
	unsigned long result = 0;

	FILE* file = fopen(fileName, "r");

	if(file)
	{
		int i;
		PlyFile *ply;
		int nprops;
		int num_elems;
		PlyProperty **plist;
		char *elem_name;
		float version;

		/*** Read in the original PLY object ***/

		ply  = ply_read (file, &nelems, &elist);
		ply_get_info (ply, &version, &file_type);

		for (i = 0; i < nelems; i++) {

			/* get the description of the first element */
			elem_name = elist[i];
			plist = ply_get_element_description (ply, elem_name, &num_elems, &nprops);

			if (equal_strings ("face", elem_name)) {
				std::cout << num_elems << std::endl;
				result = num_elems;
			}
			else
				other_elements = ply_get_other_element (ply, elem_name, num_elems);
		}

		comments = ply_get_comments (ply, &num_comments);
		obj_info = ply_get_obj_info (ply, &num_obj_info);

		ply_close (ply);

		delete[] other_elements;
	}

	return result;
}


