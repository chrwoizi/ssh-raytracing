/**
* \file ModelParser.h
*
* \brief Declaration for class ModelParser
*/


#ifndef MODELPARSER_H
#define MODELPARSER_H

#include <cassert>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <list>
#include "HashMap.hpp"

#include <GL/glut.h>

//#include <Mesh.h>
#include "vecmath.h"

using std::string;
using std::ifstream;
using std::istringstream;
using std::list;
using std::stringstream;
using std::vector;

//using alg::vec2;
//using alg::vec3;

typedef vec vec2;
typedef vec vec3;

list<string> getSingleWords(string line);


#ifndef _int3_def
#define _int3_def
struct int3{
	int data[3];
	int3(int i1, int i2, int i3) {data[0] = i1; data[1] = i2; data[2] = i3;}
	int3() {}
};
#endif

class PLYdata{
public:
	PLYdata() : vertices(0), normals(0), triangles(0), vertexProperties(0), normalProperties(0), triangleProperties(0) {};
	int vertices;
	int normals;
	int triangles;

	int vertexProperties;
	int normalProperties;
	int triangleProperties;

	list<string> order;

};

class OFFdata{
public:
	OFFdata() : vertices(0), triangles(0){};
	int vertices;
	int triangles;
};

struct MtlMaterial
{
	string name;		// relative path
	float d;		// dissolve factor (opacity)
	float Ni;		// refraction index
	vec Ka;			// ambient reflectivity
	vec Kd;			// diffuse reflectivity
	vec Ks;			// specular reflectivity
	float Ns;		// specular exponent
	float Km;
	vec Tf;			// transmission filter
	string map_Ka;		// ambient reflectivity texture
	string map_Kd;		// diffuse reflectivity texture
	string map_Ks;		// specular reflectivity texture
	string map_Bump;	// bump map

	/*
	Illumination    Properties that are turned on in the
	model           Property Editor
 
	0		Color on and Ambient off							// handled by ModelParser by setting Ka=(0,0,0) and Ks=(0,0,0)
	1		Color on and Ambient on								// handled by ModelParser by setting Ks=(0,0,0)
	2		Highlight on
	3		Reflection on and Ray trace on
	4		Transparency: Glass on
			Reflection: Ray trace on
	5		Reflection: Fresnel on and Ray trace on
	6		Transparency: Refraction on
			Reflection: Fresnel off and Ray trace on
	7		Transparency: Refraction on
			Reflection: Fresnel on and Ray trace on
	8		Reflection on and Ray trace off
	9		Transparency: Glass on
			Reflection: Ray trace off
	10		Casts shadows onto invisible surfaces
	 */
	int illum;

	MtlMaterial()
	{
		d = 0;
		Ni = 0;
		Ka = vec(0,0,0);
		Kd = vec(0,0,0);
		Ks = vec(0,0,0);
		Tf = vec(0,0,0);
		Ns = 0;
		Km = 0;
		illum = 2;
	}
};


typedef std::vector<vec3>::const_iterator constvec3iterator;
typedef std::vector<int3>::const_iterator constint3iterator;
typedef std::vector<vec3>::iterator vec3iterator;
typedef std::vector<int3>::iterator int3iterator;

/**
* \class ParseException
*
* \brief Exception class for parser
*/
class ParseException{
public:
	ParseException(string me){ m = me;};
	const string message() const{ return m; };
	string m;
};

/**
* \class ModelParser
* \brief Parses different file formats for loading 3D Objects into OpenGL applications
*
* Parses different file formats for loading 3D Objects into OpenGL 
* applications and provides these information in simple std<vectors>
* or as a display list. Objects are repositioned so that their center is
* at (0,0,0) and they fit into the unit cube [-0.5, 0.5]^3
*
* \note 
* requires QT
* currently supported file formats are
* - PLY, partly, vertices, facenormals, and triangles are supported
* 
* \author 
* - Martin Eisemann (ME), eisemann@cg.cs.tu-bs.de
*
* \date   
* - Created 2006-11-06 (ME)
*
*/
class ModelParser
{
public:
	ModelParser();
	~ModelParser();

	vector<vec3> getVertices();
	vector<vec3> getVertexNormals();
	vector<vec3> getTriangleNormals();
	vector<vec3> getTextureCoordinates();
	HashMap<int, int> getMaterialIds();
	vector<MtlMaterial> getMaterials();
	vector<int3> getTriangles();
	void getBoundingBox(vec3& min, vec3& max) const;
	void getTranslationAndScaleForUnitCube(float translate[3], float& scale) const;
	void getModelMatrix(float matrix[16]);
	void getTranslationMatrix(float matrix[16]);
	//Martin::Mesh getMesh();

	bool  loadFile(const string fileName, const bool useVertexNormals = false);
	unsigned long getFaceCount(const string fileName);
	GLint createDisplayList() const;  

protected:
	// PLY files //
	void loadPLYfile(const string fileName);
	void parsePLYHeader(ifstream& file, PLYdata& plydata);
	int  getAmountOfPLYProperties(ifstream& file);
	void parsePLYHeaderVertex(list<string>& words, ifstream& file, PLYdata& plydata);    
	void parsePLYHeaderFace(list<string>& words, ifstream& file, PLYdata& plydata);    
	void parsePLYBody(ifstream& file, PLYdata& plydata);
	void parsePLYBodyVertex(ifstream& file, PLYdata& plydata);
	void parsePLYBodyFace(ifstream& file, PLYdata& plydata);

	// common functions //
	void createTriangleNormals();
	vec3 createTriangleNormal(const int3& triangle) const;
	void createVertexNormals();
	bool checkTriangle(int3 triangle);
	void checkData();
	const float  calcTriangleArea(const unsigned int tri) const;
	const vec3 calcVertexNormal(const vector<int>& adjacentTriangles, const vector<float>& triArea) const;

	// OFF files //
	void loadOFFfile(const string fileName);
	unsigned long getOFFFaceCount(const string fileName);
	void parseOFFHeader(ifstream& file, OFFdata& offdata);
	void parseOFFBody(ifstream& file, OFFdata& offdata);

	// NTRI files //
	void loadNTRIfile(const string fileName);

	// OBJ files //
	void loadOBJfile(const string fileName);
	void parseMtlLib(const string& fileName, vector<MtlMaterial>& materials);
	unsigned long getOBJFaceCount(const string fileName);
	void parseOBJface(istringstream& iss,vector<int3>& triVerts, vector<int3>& triNorms, 
		vector<int3>& triTexCoords, vector<int>& triMaterialIds, unsigned int vertexCount, unsigned int texCoordCount, unsigned int normalCount);
	void triangulateOBJFace(const vector<int>& verts, 
		const vector<int>& norms, 
		const vector<int>& texCoords, 
		vector<int3>& triVerts, 
		vector<int3>& triNorms, 
		vector<int3>& triTexCoords,
		vector<int>& triMaterialIds);
	void convertOBJinfos(const vector<vec3>& verticess, 
		const vector<vec3>& normalss, 
		const vector<vec2>& texCoords, 
		const vector<int3>& triVerts, 
		const vector<int3>& triNorms, 
		const vector<int3>& triTexCoords,
		vector<int>& materialIds);


	/**
	* \brief Contains the vertices
	*/
	vector<vec3> mVertices;

	/**
	* \brief Contains the vertex normals
	*/
	vector<vec3> mVertexNormals;

	/**
	* \brief Contains the triangle normals
	*/
	vector<vec3> mTriangleNormals;

	/**
	* \brief Contains the texture coordinates
	*/
	vector<vec3> mTextureCoordinates;

	/**
	* \brief Contains the correspondences between triangles and material ids
	*/
	HashMap<int, int> mMaterialIds;

	/**
	* \brief contains the triangles, with indices to the vertices (indices are already multiplied by three, to get the correct index into the vertex array)
	*/
	vector<int3> mTriangles;

	/**
	* \brief contains the materials. use mMaterialIds for access
	*/
	vector<MtlMaterial> mMaterials;

	/**
	* \brief state variable depicting if vertex normals should be used
	*/
	bool mUseVertexNormals;

	/**
	* \brief state variable depicting if texture or material information is available
	*/
	bool mHasTextureOrMaterial;

	/**
	* \brief state variable depicting if lighting information is available
	*/
	bool mHasLights;

	/**
	* \brief Saves the line currently parsed
	*/
	int mLine;

	/**
	* \brief Saves the current material id
	*/
	int mCurrentMaterial;

	/**
	* \brief for simplicity
	*/
	enum dim{X, Y, Z};
};

#endif   // !MODELPARSER_H
