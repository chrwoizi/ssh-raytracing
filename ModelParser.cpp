/**
* \file ModelParser.cpp
*
* \brief Definition for methods of class ModelParser
*/

#include <iostream>
#include <cfloat>
#include <fstream>

#include "ModelParser.hpp"

using std::ifstream;
using std::list;
using std::vector;
using std::cout;
using std::endl;

/**
* \brief Empty Constructor
*/ 
ModelParser::ModelParser()
: mUseVertexNormals(false), mHasTextureOrMaterial(false), mHasLights(false)
{

}

/**
* \brief Destructor
*/
ModelParser::~ModelParser()
{

}


/**
* \brief Returns the vertices of the model as an array containing floats (3 for one vertex)
*
* \return
* - array containing the vertices of the vector
*/
std::vector<vec3> ModelParser::getVertices()
{
	return mVertices;
}


/**
* \brief Returns the normals of the vertices of the model as an array, elements are floats (3 floats for one vertexnormal)
*
* \return
* - array containing the normal vectors of the vertices
*/
std::vector<vec3> ModelParser::getVertexNormals()
{
	return mVertexNormals;
}


/**
* \brief Returns the normals of the vertices of the model as an array, elements are floats (3 floats for one vertexnormal)
*
* \return
* - array containing the normal vectors of the vertices
*/
std::vector<vec3> ModelParser::getTextureCoordinates()
{
	return mTextureCoordinates;
}


/**
* \brief Returns the normals of the triangles of the model as an array, elements are floats (3 for 1 triangle normal)
*
* \return
* - array containing the normal vectors of the triangles
*/
std::vector<vec3> ModelParser::getTriangleNormals()
{
	return mTriangleNormals;
}


/**
* \brief Returns the correspondences between triangles of the model and material ids. keys are triangle idd, values are material ids
*/
HashMap<int, int> ModelParser::getMaterialIds()
{
	return mMaterialIds;
}


/**
* \brief Returns the Triangles of the model as an array, elements are ints pointing to the vertices (have to be multiplied by 3)
*
* \return
* - array containing the triangles of the vector
*/
std::vector<int3> ModelParser::getTriangles()
{
	return mTriangles;
}

/**
* \brief Returns the Materials of the model as an array.
*/
vector<MtlMaterial> ModelParser::getMaterials()
{
	return mMaterials;
}

/**
* \brief Returns the axis-aligned bounding box of the current object
*
* \param min - minimum xyz values of the bounding box
* \param max - maximum xyz values of the bounding box
*
* \note
* - works on the vertices, in corrupted models, some of them may not be use
*/
void ModelParser::getBoundingBox(vec3& min, vec3& max) const
{
	min[X] = FLT_MAX;
	min[Y] = FLT_MAX;
	min[Z] = FLT_MAX;

	max[X] = -FLT_MAX;
	max[Y] = -FLT_MAX;
	max[Z] = -FLT_MAX;

	for(constvec3iterator iter = mVertices.begin(); iter < mVertices.end(); iter++){ 
		const vec3& value = *iter;

		if(min[X] > value[X]) min[X] = value[X];
		if(min[Y] > value[Y]) min[Y] = value[Y];
		if(min[Z] > value[Z]) min[Z] = value[Z];

		if(max[X] < value[X]) max[X] = value[X];
		if(max[Y] < value[Y]) max[Y] = value[Y];
		if(max[Z] < value[Z]) max[Z] = value[Z];   
	}
}

/**
* \brief Returns the translation and scale factors, so that the model fits into the cube [-0.5, 0.5]^3
*/
void ModelParser::getTranslationAndScaleForUnitCube(float translate[3], float& scale) const
{
	vec3 min, max;
	getBoundingBox(min, max);

	float extend[] = { max[0]-min[0],
		max[1]-min[1],
		max[2]-min[2]};

	translate[0] = -min[0] - extend[0]/2.0;
	translate[1] = -min[1] - extend[1]/2.0;
	translate[2] = -min[2] - extend[2]/2.0;


	scale = extend[0];
	if(extend[1] > scale) scale = extend[1];
	if(extend[2] > scale) scale = extend[2];

	scale = 1.0f/scale;
}


/**
* \brief Returns the modelmatrix of the model (only one supported)
*/
void ModelParser::getModelMatrix(float matrix[16])
{
	float translate[3];
	float scale;
	getTranslationAndScaleForUnitCube(translate, scale);

	GLint matrix_mode;
	glGetIntegerv(GL_MATRIX_MODE, &matrix_mode);
	switch(matrix_mode){
  case GL_MODELVIEW:
	  matrix_mode = GL_MODELVIEW_MATRIX;
	  break;
  case GL_TEXTURE:
	  matrix_mode = GL_TEXTURE_MATRIX;
	  break;
  case GL_PROJECTION:
	  matrix_mode = GL_PROJECTION_MATRIX;
	  break;
  default:
	  std::cout << "Error, no supported matrix mode";
	  break;
	}
	glPushMatrix();
	glLoadIdentity();
	glScalef(scale, scale, scale);
	glTranslatef(translate[0], translate[1], translate[2]);

	//glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	glGetFloatv(matrix_mode, matrix);

	glPopMatrix();  
}


/**
* \brief Returns the translationmatrix of the model (only one supported)
*/
void ModelParser::getTranslationMatrix(float matrix[16])
{
	float translate[3];
	float scale;
	getTranslationAndScaleForUnitCube(translate, scale);

	GLint matrix_mode;
	glGetIntegerv(GL_MATRIX_MODE, &matrix_mode);
	switch(matrix_mode){
  case GL_MODELVIEW:
	  matrix_mode = GL_MODELVIEW_MATRIX;
	  break;
  case GL_TEXTURE:
	  matrix_mode = GL_TEXTURE_MATRIX;
	  break;
  case GL_PROJECTION:
	  matrix_mode = GL_PROJECTION_MATRIX;
	  break;
  default:
	  std::cout << "Error, no supported matrix mode";
	  break;
	}
	glPushMatrix();
	glLoadIdentity();
	//glScalef(scale, scale, scale);
	glTranslatef(translate[0], translate[1], translate[2]);

	glGetFloatv(matrix_mode, matrix);

	glPopMatrix();  
}


/**
* \brief Returns a Mesh, ready for rendering
*/
/*Martin::Mesh ModelParser::getMesh()
{
Martin::Mesh mesh;

vector<vec3> & vertices = mVertices;
vector<vec3> & normals  = mVertexNormals;
vector<int3> & triangles = mTriangles;

mesh.vertices.resize(vertices.size()*3);
for(unsigned int i = 0; i < vertices.size(); ++i) {
mesh.vertices[i*3+0] = vertices[i][0];
mesh.vertices[i*3+1] = vertices[i][1];
mesh.vertices[i*3+2] = vertices[i][2];
}
mesh.normals.resize(normals.size()*3);
for(unsigned int i = 0; i < normals.size(); ++i) {
mesh.normals[i*3+0] = normals[i][0];
mesh.normals[i*3+1] = normals[i][1];
mesh.normals[i*3+2] = normals[i][2];
}
mesh.triangles.resize(triangles.size()*3);
for(unsigned int i = 0; i < triangles.size(); ++i) {
mesh.triangles[i*3+0] = triangles[i].data[0];
mesh.triangles[i*3+1] = triangles[i].data[1];
mesh.triangles[i*3+2] = triangles[i].data[2];
}

return mesh;
}*/


/**
* \brief loads an object from a file
*/
bool ModelParser::loadFile(const string s, const bool useVertexNormals)
{ 
	if(s == "") return false;

	mUseVertexNormals = useVertexNormals;

	mLine = 0;

	/* load data from file */
	string ending = s.substr(s.rfind('.' , s.size()-1));
	if(ending == ".ply"){
		try{
			loadPLYfile(s);
		}
		catch(const ParseException& exception){
			std::cout << exception.message();
			return false;
		}
	}
	else if(ending == ".off"){
		try{
			loadOFFfile(s);
		}
		catch(const ParseException& exception){
			std::cout << exception.message();
			return false;
		}
	}
	else if(ending == ".ntri"){
		try{
			loadNTRIfile(s);
		}
		catch(const ParseException& exception){
			std::cout << exception.message();
			return false;
		}
	}
	else if(ending == ".obj"){
		try{
			loadOBJfile(s);
		}
		catch(const ParseException& exception){
			std::cout << exception.message();
			return false;
		}
	}

	else // default
		return false;

	checkData();

	try{
		if((mVertexNormals.size() == 0) && (mTriangleNormals.size() == 0)){
			createTriangleNormals();   
		}

		if(mVertexNormals.size() == 0 && mUseVertexNormals){
			createVertexNormals();
		}
	}
	catch(const ParseException& exception){
		std::cout << exception.message();
		return false;
	}

	return true;
}

unsigned long ModelParser::getFaceCount(const string s)
{
	if(s == "") return 0;

	mLine = 0;

	/* load data from file */
	string ending = s.substr(s.rfind('.' , s.size()-1));
	if(ending == ".ply"){
		try{
			loadPLYfile(s);
			return mTriangles.size();
		}
		catch(const ParseException& exception){
			std::cout << exception.message();
			return 0;
		}
	}
	else if(ending == ".off"){
		try{
			return getOFFFaceCount(s);
		}
		catch(const ParseException& exception){
			std::cout << exception.message();
			return 0;
		}
	}
	else if(ending == ".ntri"){
		try{
			loadNTRIfile(s);
			return mTriangles.size();
		}
		catch(const ParseException& exception){
			std::cout << exception.message();
			return 0;
		}
	}
	else if(ending == ".obj"){
		try{
			return getOBJFaceCount(s);
		}
		catch(const ParseException& exception){
			std::cout << exception.message();
			return 0;
		}
	}
	else // default
		return 0;
}


/**
* \brief Creates an OpenGL display list of the current data
*/
GLint ModelParser::createDisplayList() const
{
	assert((mTriangles.size() == mTriangleNormals.size()) || (mVertices.size() == mVertexNormals.size()));

	GLint result = glGenLists(1);
	glNewList(result, GL_COMPILE);

	/* lights */
	if(mHasLights){;} //not implemented yet


	/* geometry */
	glBegin(GL_TRIANGLES);
	int count = mTriangles.size();
	for(int i=0; i<count; i++){
		int3 index = mTriangles.at(i);
		vec3 p0  = mVertices.at(index.data[0]);
		vec3 p1  = mVertices.at(index.data[1]);
		vec3 p2  = mVertices.at(index.data[2]);

		if(!mUseVertexNormals){
			vec3 n0 = mTriangleNormals.at(i);

			glNormal3f(n0[X], n0[Y], n0[Z]);
			glVertex3f(p0[X], p0[Y], p0[Z]);
			glVertex3f(p1[X], p1[Y], p1[Z]);
			glVertex3f(p2[X], p2[Y], p2[Z]);
		}
		else{
			vec3 n0 = mVertexNormals.at(index.data[0]);
			vec3 n1 = mVertexNormals.at(index.data[1]);
			vec3 n2 = mVertexNormals.at(index.data[2]);

			glNormal3f(n0[X], n0[Y], n0[Z]);
			glVertex3f(p0[X], p0[Y], p0[Z]);

			glNormal3f(n1[X], n1[Y], n1[Z]);
			glVertex3f(p1[X], p1[Y], p1[Z]);

			glNormal3f(n2[X], n2[Y], n2[Z]);
			glVertex3f(p2[X], p2[Y], p2[Z]);
		}
	}
	glEnd();

	glEndList();

	return result;
}


/**
* \brief Loads an object model from a PLY-file (Stanford Model File) into the member vectors
*/
void ModelParser::loadPLYfile(const string fileName)
{
	PLYdata plydata;
	string fn = fileName;

	std::cout << "Loading file: " << fn << "\n\n";

	ifstream file(fileName.c_str());

	if(!file.good()) throw ParseException("Couldn't open file.\n");

	parsePLYHeader(file, plydata);

	parsePLYBody(file, plydata);

	file.close();
}


/**
* \brief Parses the Header of a PLY file
*
* \param file - the current file
* \param plydata - the helper structure for ply files
*/
void ModelParser::parsePLYHeader(ifstream& file, PLYdata& plydata)
{  
	string line;

	while (line != "end_header" && !file.eof()) {
		getline(file, line);
		++mLine;

		list<string> words;


		// break into single words //
		words = getSingleWords(line);

		// hier weiter
		while(!words.empty()){
			string temp = words.front();
			words.pop_front();

			// format //
			if(temp == "format"){
				temp = words.front();
				words.pop_front();
				if(temp != "ascii"){	  
					throw ParseException("No other format than ascii supported in this version. Loading failed.\n");
				}
			}

			// element //
			else if(temp == "element"){
				temp = words.front();
				words.pop_front();

				// vertices //
				if(temp == "vertex"){
					parsePLYHeaderVertex(words, file, plydata);
					plydata.order.push_back(temp);
				}

				// face //
				if(temp == "face"){
					parsePLYHeaderFace(words, file, plydata);
					plydata.order.push_back(temp);
				}
			}
			// nothing which is supported //
			else{
				words.clear();
			}
		}  
	} 
}


/**
* \brief Returns the amount of properties read in from file
*
* \param file - the current file
*
* \return
* - returns amount of properties
*/
int ModelParser::getAmountOfPLYProperties(ifstream& file)
{
	int result = 0;
	string line;
	char word[8];

	file.get(word, 9);
	string temp = word;
	while(temp == "property"){
		getline(file, line);
		++mLine;
		file.get(word, 9);
		temp = word;
		result ++;
	}

	for(int i=0; i<8; i++){
		file.unget();
	}

	return result;
}


/**
* \brief Parse number of vertices and properties
*
* \param words - the words of the current line
* \param file - the current file
* \param plydata - the helper structure for ply files
*/
void ModelParser::parsePLYHeaderVertex(list<string>& words, 
									   ifstream& file, PLYdata& plydata)
{
	string temp = words.front();	  
	words.pop_front();
	plydata.vertices = atoi(temp.c_str());

	std::cout << "Number of Vertices: " << plydata.vertices << "\n";
	words.clear(); // throw away the rest of the line  

	/* get amount of vertex properties */
	plydata.vertexProperties = getAmountOfPLYProperties(file);
	if(plydata.vertexProperties < 3) 
		throw ParseException("Not enough vertex properties.\n");
}


/**
* \brief Parse number of triangles and properties
*
* \param words - the words of the current line
* \param file - the current file
* \param plydata - the helper structure for ply files
*/
void ModelParser::parsePLYHeaderFace(list<string>& words, 
									 ifstream& file, PLYdata& plydata)
{
	string temp = words.front();	  
	words.pop_front();
	plydata.triangles = atoi(temp.c_str());


	std::cout << "Number of Triangles: " << plydata.triangles << "\n";
	words.clear(); // throw away the rest of the line

	/* get amount of vertex properties */
	plydata.triangleProperties = getAmountOfPLYProperties(file);

	if(plydata.triangleProperties != 1) 
		throw ParseException("Too much or to less face properties.\n");
}


/**
* \brief Parses the Body of a PLY file
*
* \param file - the current file
* \param plydata - the helper structure for ply files
*/
void ModelParser::parsePLYBody(ifstream& file, PLYdata& plydata){

	while(!plydata.order.empty()){
		string temp = plydata.order.front();
		plydata.order.pop_front();

		if(temp == "vertex")
			parsePLYBodyVertex(file, plydata);
		else if(temp == "face")
			parsePLYBodyFace(file, plydata);
	}    
}


/**
* \brief Parses the Body of a PLY file for vertices
*
* \param file - the current file
* \param plydata - the helper structure for ply files
*/
void ModelParser::parsePLYBodyVertex(ifstream& file, PLYdata& plydata)
{  
	for(int i=0; i<plydata.vertices; i++){
		string line;
		getline(file, line);
		++mLine;

		list<string> words = getSingleWords(line);


		vec3 vertex;
		vertex[0] = atof(words.front().c_str());
		words.pop_front();
		vertex[1] = atof(words.front().c_str());
		words.pop_front();
		vertex[2] = atof(words.front().c_str());
		words.pop_front();

		words.clear();

		mVertices.push_back(vertex);
	}
}

/**
* \brief Parses the Body of a PLY file for faces
*
* \param file - the current file
* \param plydata - the helper structure for ply files
*/
void ModelParser::parsePLYBodyFace(ifstream& file, PLYdata& plydata)
{
	for(int i=0; (i<plydata.triangles) && (!file.eof()); i++){
		string line;
		getline(file, line);
		mLine++;

		list<string> words = getSingleWords(line);
		assert(words.size() == 4);

		int3 face;
		int amount;

		amount = atoi(words.front().c_str());
		words.pop_front();
		if(amount != 3) throw ParseException("No primitives except for triangles supported.\n");

		face.data[0] = atoi(words.front().c_str());
		words.pop_front();
		face.data[1] = atoi(words.front().c_str());
		words.pop_front();
		face.data[2] = atoi(words.front().c_str());
		words.pop_front();

		words.clear();

		if(checkTriangle(face))
			mTriangles.push_back(face);
	}
}


/**
* \brief Creates normals for all triangles
*/
void ModelParser::createTriangleNormals()
{
	mTriangleNormals.clear();

	for(constint3iterator iter = mTriangles.begin(); iter < mTriangles.end(); iter++){
		int3 triangle = *iter;

		mTriangleNormals.push_back(createTriangleNormal(triangle));
	}
}


/**
* \brief Create a normal for a triangle
*
* The normal is calculated assuming counter-clockwise defined triangle
*
* \param triangle - indices to the vertices of the triangle
*
* \return normalized normalvector of the triangle
*/
vec3 ModelParser::createTriangleNormal(const int3& triangle) const
{
	vec3 p0 = mVertices.at(triangle.data[0]);
	vec3 p1 = mVertices.at(triangle.data[1]);
	vec3 p2 = mVertices.at(triangle.data[2]);

	vec3 mPlaneV1 = p1-p0;
	vec3 mPlaneV2 = p2-p0;

	return (mPlaneV1^mPlaneV2).normalize();
}


/**
* \brief Creates normals for all vertices
*/
void ModelParser::createVertexNormals()
{
	assert(mTriangleNormals.size() == mTriangles.size());

	mVertexNormals.clear();
	mVertexNormals.reserve(mVertices.size());
	const unsigned int numTris  = mTriangles.size();
	const unsigned int numVerts = mVertices.size();

	mVertexNormals.resize(mVertices.size());
	for(vec3iterator iter = mVertexNormals.begin(); iter < mVertexNormals.end(); iter++){
		*iter = vec3(0.0f, 0.0f, 0.0f);
	}

	float* sumWeight = new float[numVerts];

	for(unsigned int i=0; i<numVerts; i++)
		sumWeight[i] = 0.0f;

	for(unsigned int i=0; i<numTris; i++){
		const int3& triangle = mTriangles.at(i);
		const float triArea = calcTriangleArea(i);

		const int v0index = triangle.data[0];
		const int v1index = triangle.data[1];
		const int v2index = triangle.data[2];

		mVertexNormals.at(v0index) = mVertexNormals.at(v0index) + triArea *  mTriangleNormals.at(i);
		sumWeight[v0index] += triArea;

		mVertexNormals.at(v1index) = mVertexNormals.at(v1index) + triArea * mTriangleNormals.at(i);
		sumWeight[v1index] += triArea;

		mVertexNormals.at(v2index) = mVertexNormals.at(v2index) + triArea * mTriangleNormals.at(i);
		sumWeight[v2index] += triArea;
	}

	for(unsigned int i=0; i<numVerts; i++){
		mVertexNormals.at(i) = mVertexNormals.at(i) / sumWeight[i];
	}

	delete[] sumWeight;
}


/**
* \brief Checks for degenerated triangles and removes them
*/
bool ModelParser::checkTriangle(int3 triangle)
{
	if((triangle.data[0] >= (int)mVertices.size()) ||
		(triangle.data[1] >= (int)mVertices.size()) ||
		(triangle.data[2] >= (int)mVertices.size())){
			throw ParseException("Triangle Data corrupted. Index out of bounds.");
	}

	/* check for degenerated triangles */
	vec3 v0 = mVertices.at(triangle.data[0]);
	vec3 v1 = mVertices.at(triangle.data[1]);
	vec3 v2 = mVertices.at(triangle.data[2]);

	if((v0 == v1) || (v0 == v2) || (v1 == v2))
		return false;

	return true;
}


/**
* \brief Checks the data for failures and removes them (hopefully)
*/
void ModelParser::checkData()
{
	for(unsigned int i=0; i<mTriangles.size(); i++){
		int3 triangle = mTriangles.at(i);
		if(!checkTriangle(triangle)){
			cout << "Triangle number " << i << " was corrupted. Removed it.\n";
			int3iterator iter = mTriangles.begin() + i;
			mTriangles.erase(iter);
			i--;
		}
	}
}


/**
* \brief splits up a line into single words
*/
std::list<string> getSingleWords(string line)
{
	std::list<string> result;

	while(line != ""){
		int index = line.find(" ");

		if(index == -1){ // just one word left
			result.push_back(line);
			break;
		}
		else{
			string temp = line;
			temp.erase(index);
			result.push_back(temp);

			line.erase(0, index+1);
		}
	}
	return result;
}


/**
* \brief calculates the are of a triangle
*
* \param tri - index to the triangle
*
* \return
* - area of the triangle
*/
const float ModelParser::calcTriangleArea(const unsigned int tri) const{
	if(tri > mTriangles.size())
		return 0.0f;


	float result = 0.0f;
	int3 triangle = mTriangles.at(tri);


	vec3 v0 = mVertices.at(triangle.data[0]);
	vec3 v1 = mVertices.at(triangle.data[1]);
	vec3 v2 = mVertices.at(triangle.data[2]);

	vec3 dir1 = v1 - v0;
	vec3 dir2 = v2 - v0;

	result = (dir1 ^ dir2).length() / 2.0f;

	assert(result >= 0.0f);

	return result;
}


/**
* \brief Calculate the normal for a vertex, based on the areas of the adjacent triangles
*
* \param adjacentTriangles - indices of the adjacent triangles
* \param triArea - array with all areas of the triangles
* \param vIndex - index of the current vertex
*
* \return
* - normalized normal of the vertex
*/
const vec3 ModelParser::calcVertexNormal(const vector<int>& adjacentTriangles, 
										 const vector<float>& triArea) const
{
	vec3 result(0.0f, 0.0f, 0.0f);
	float sumArea = 0;
	const int numTris = adjacentTriangles.size();
	float* weight = new float[numTris];
	vec3* triNormals = new vec3[numTris];

	if(numTris == 0)
		return vec3(0.0f, 1.0f, 0.0f);

	// set weights for adjacent triangles and calc normal
	for(int i=0; i<numTris; i++){
		const int triIndex = adjacentTriangles.at(i);
		weight[i] = triArea.at(triIndex);
		sumArea += weight[i];

		triNormals[i] = mTriangleNormals.at(triIndex);
	}
	for(int i=0; i<numTris; i++){
		weight[i] /= sumArea;
	}

	// calc weighted normal
	for(int i=0; i<numTris; i++){
		result = result + (weight[i] * triNormals[i]);
	}

	delete[] weight;
	delete[] triNormals;

	return result;
}


/**
* \brief loads an OFF file mesh
*
* \param fileName - name of the OFF file
*/
void ModelParser::loadOFFfile(const string fileName)
{
	string fn = fileName;

	//std::cout << "Loading file: " << fn << "\n\n";

	ifstream file(fileName.c_str());

	if(!file.good()) throw ParseException("Couldn't open file.\n");

	OFFdata offdata;

	parseOFFHeader(file, offdata);

	parseOFFBody(file, offdata);

	file.close();
}

unsigned long ModelParser::getOFFFaceCount(const string fileName)
{
	string fn = fileName;

	//std::cout << "Loading file: " << fn << "\n\n";

	ifstream file(fileName.c_str());

	if(!file.good()) throw ParseException("Couldn't open file.\n");

	OFFdata offdata;

	parseOFFHeader(file, offdata);

	file.close();

	return offdata.triangles;
}


/**
* \brief parse the header of an OFF file
*/
void ModelParser::parseOFFHeader(ifstream& file, OFFdata& offdata)
{
	string s;
	int unknown;

	getline(file, s); //contains the off
	++mLine;

	string::size_type loc = s.find("OFF");

	if(loc == string::npos)
		throw ParseException("No OFF file. Error in header.\n");

	getline(file, s);
	++mLine;
	istringstream ss(s);

	ss >> offdata.vertices;
	ss >> offdata.triangles;
	ss >> unknown;

}


/**
* \brief parse the body of an OFF file
*/
void ModelParser::parseOFFBody(ifstream& file, OFFdata& offdata)
{
	// parse vertices
	for(int i=0; i<offdata.vertices; ++i){
		vec3 temp;
		string sTemp;
		getline(file, sTemp);
		++mLine;

		istringstream ss(sTemp);
		ss >> temp[0] >> temp[1] >> temp[2];
		mVertices.push_back(temp);
	}

	// parse triangles
	for(int i=0; i<offdata.triangles; i++){
		int3 temp;
		int prim;
		string sTemp;
		getline(file, sTemp);
		++mLine;

		istringstream ss(sTemp);

		ss >> prim;
		if(prim != 3)
			throw ParseException("Unsupported Primitive, no triangle.");

		ss >> temp.data[0] >> temp.data[1] >> temp.data[2];
		mTriangles.push_back(temp);
	}
}


/**
* \brief loads an ntri file mesh
*
* \param fileName - name of the NTRI file
*/
void ModelParser::loadNTRIfile(const string fileName)
{
	string fn = fileName;

	std::cout << "Loading file: " << fn << "\n\n";

	ifstream file(fileName.c_str());

	if(!file.good()) throw ParseException("Couldn't open file.\n");

	int numVertices, numTriangles;

	file >> numVertices;
	file >> numTriangles;

	// parse vertices abd normals
	for(int i=0; i<numVertices; i++){
		vec3 temp;
		file >> temp[0] >> temp[1] >> temp[2];
		mVertices.push_back(temp);

		file >> temp[0] >> temp[1] >> temp[2];
		mVertexNormals.push_back(temp);
	}

	// parse triangles
	for(int i=0; i<numTriangles; i++){
		int3 temp;
		int prim;
		file >> prim;
		if(prim != 3)
			throw ParseException("Unsupported Primitive, no triangle.");

		file >> temp.data[0] >> temp.data[1] >> temp.data[2];
		mTriangles.push_back(temp);
	}

	// activate vertex normals
	mUseVertexNormals = true;

	file.close();
}

void ModelParser::parseMtlLib(const string& fileName, vector<MtlMaterial>& materials)
{
	mMaterials.clear();

	string fn = fileName;
	std::cout << "Loading file: " << fn << "\n\n";

	ifstream file(fn.c_str());
	if(!file.good())
	{
		cout << "Couldn't open file." << endl;
		return;
	}

	string line;

	MtlMaterial* currentMaterial = NULL;
	while(getline(file, line))
	{ 
		istringstream iss(line);
		string key;
		iss >> key;
		if(key == "newmtl")
		{
			materials.resize(materials.size()+1);
			currentMaterial = &(*materials.rbegin());
			iss >> currentMaterial->name;
		}
		else if(key == "d")
		{
			assert(currentMaterial);
			iss >> currentMaterial->d;
		}
		else if(key == "Ni")
		{
			assert(currentMaterial);
			iss >> currentMaterial->Ni;
		}
		else if(key == "Ka")
		{
			assert(currentMaterial);
			iss >> currentMaterial->Ka[0] >> currentMaterial->Ka[1] >> currentMaterial->Ka[2];
		}
		else if(key == "Kd")
		{
			assert(currentMaterial);
			iss >> currentMaterial->Kd[0] >> currentMaterial->Kd[1] >> currentMaterial->Kd[2];
		}
		else if(key == "Ks")
		{
			assert(currentMaterial);
			iss >> currentMaterial->Ks[0] >> currentMaterial->Ks[1] >> currentMaterial->Ks[2];
		}
		else if(key == "Ns")
		{
			assert(currentMaterial);
			iss >> currentMaterial->Ns;
		}
		else if(key == "Km")
		{
			assert(currentMaterial);
			iss >> currentMaterial->Km;
		}
		else if(key == "Tf")
		{
			assert(currentMaterial);
			iss >> currentMaterial->Tf[0] >> currentMaterial->Tf[1] >> currentMaterial->Tf[2];;
		}
		else if(key == "map_Ka")
		{
			assert(currentMaterial);
			iss >> currentMaterial->map_Ka;
		}
		else if(key == "map_Kd")
		{
			assert(currentMaterial);
			iss >> currentMaterial->map_Kd;
		}
		else if(key == "map_Ks")
		{
			assert(currentMaterial);
			iss >> currentMaterial->map_Ks;
		}
		else if(key == "map_Bump")
		{
			assert(currentMaterial);
			iss >> currentMaterial->map_Bump;
		}
		else if(key == "illum")
		{
			assert(currentMaterial);
			iss >> currentMaterial->illum;
		}
	}

	for(unsigned int i = 0; i < materials.size(); ++i)
	{
		if(materials[i].illum == 0)
		{
			materials[i].Ka = vec(0,0,0);
			materials[i].Ks = vec(0,0,0);
		}
		else if(materials[i].illum == 1)
		{
			materials[i].Ks = vec(0,0,0);
		}
	}
	
	std::cout << "Done loading file: " << fn << "\n\n";
}

/**
* \brief loads an OBJ file mesh
*
* \param fileName - name of the OBJ file
*/
void ModelParser::loadOBJfile(const string fileName)
{
	vector<vec3> vertices;
	vector<vec3> normals;
	vector<vec2> texCoords;
	vector<int3> triVerts;
	vector<int3> triNorms;
	vector<int3> triTexCoords;
	vector<int> materialIds;
	mCurrentMaterial = 0;

	string fn = fileName;
	std::cout << "Loading file: " << fn << "\n\n";

	ifstream file(fn.c_str());
	if(!file.good()) throw ParseException("Couldn't open file.\n");

	string line;

	while(getline(file, line)){  
		++mLine;
		istringstream iss(line);
		string key;
		iss >> key;
		if(key == "v"){
			// parse vertex //
			vec3 v;
			iss >> v[0] >> v[1] >> v[2];
			vertices.push_back(v);
		}
		else if(key == "vt"){
			// parse texture coordinate //
			vec2 tc;
			iss >> tc[0] >> tc[1];
			texCoords.push_back(tc);
		}
		else if(key == "vn"){
			// parse vertex normal //
			vec3 n;
			iss >> n[0] >> n[1] >> n[2];
			normals.push_back(n);
		}
		else if(key == "f"){
			// parse face //
			parseOBJface(iss, triVerts, triNorms, triTexCoords, materialIds, vertices.size(), texCoords.size(), normals.size());
		}
		else if(key == "mtllib"){
			// parse material library //
			string mtllibname;
			iss >> mtllibname;

			// get directory of model file.
			string dir;
			size_t pos = fileName.find_last_of("\\");
			if(pos == string::npos)
			{
				pos = fileName.find_last_of("/");
				if(pos != string::npos)
				{
					dir = fileName.substr(0, pos+1);
				}
			}
			else
			{
				size_t pos2 = fileName.find_last_of("/");
				if(pos2 == string::npos)
				{
					dir = fileName.substr(0, pos+1);
				}
				else
				{
					dir = fileName.substr(0, (pos<pos2?pos:pos2)+1);
				}
			}

			// concat material lib filename with directory and load material lib
			parseMtlLib(dir + mtllibname, mMaterials);
		}
		else if(key == "usemtl"){
			// parse material //
			string mtlname;
			iss >> mtlname;
			for(size_t i = 0; i < mMaterials.size(); ++i)
			{
				if(mMaterials[i].name == mtlname)
				{
					mCurrentMaterial = i;
					break;
				}
			}
		}
		else{
			// anything else, treated as comments at the moment //
			continue;
		}
	}


	// all needed information is now saved in the arrays //
	// now convert it, so that it can be passed to OpenGL (since it supports only one normal per vertex) //
	convertOBJinfos(vertices, normals, texCoords, 
		triVerts, triNorms, triTexCoords, materialIds);
	
	std::cout << "Done loading file: " << fn << "\n\n";
}

unsigned long ModelParser::getOBJFaceCount(const string fileName)
{ 
	string fn = fileName;

	ifstream file(fn.c_str());
	if(!file.good()) throw ParseException("Couldn't open file.\n");

	string line;

	unsigned long result = 0;

	while(getline(file, line)){  
		++mLine;
		istringstream iss(line);
		string key;
		iss >> key;
		if(key == "f"){
			++result;
		}
		else{
			// anything else, treated as comments at the moment //
			continue;
		}
	}

	return result;
}


/**
* \brief Parse a face and triangulate if necessary
*
* Parse a face and triangulate if necessary, it is assumed that if 
* more than one value per vertex is presented, that the second one 
* is the texture coordinate index and the third one the normal index.
*
* \param iss - the input stringstream containing the data
* \param triVerts - array saving the vertex data
* \param triNorms - array saving the normal data
* \param triTexCoords - array saving the texture coordinates
*
*/
void ModelParser::parseOBJface(istringstream& iss, 
							   vector<int3>& triVerts, 
							   vector<int3>& triNorms, 
							   vector<int3>& triTexCoords,
							   vector<int>& triMaterialIds, 
							   unsigned int vertexCount, 
							   unsigned int texCoordCount,
							   unsigned int normalCount)
{
	int v, t, n;
	char skip = 0;
	int polygonCount = 0;

	vector<int> verts;
	vector<int> norms;
	vector<int> texCoords;

	while(!iss.eof()){
		iss >> v;
		if(iss.fail()) break;
		if(v < 0) v = vertexCount + v +1;
		assert(v>=0);
		verts.push_back(v);

		if(iss.peek() == '/'){     
			iss >> skip;

			// if no textures are specified //
			if(iss.peek() == '/'){
				iss >> skip >> n;
				if(iss.fail()) break;  
				if(n < 0) n = normalCount + n +1;
				assert(n>=0);
				norms.push_back(n);
			}
			else{
				iss >> t;
				if(iss.fail()) break;
				if(t < 0) t = texCoordCount + t +1;
				assert(t>=0);
				texCoords.push_back(t);
			}

			if(iss.peek() == '/'){
				iss >> skip >> n;
				if(iss.fail()) break;  
				if(n < 0) n = normalCount + n +1;
				assert(n>=0);
				norms.push_back(n);
			}
		}
		++polygonCount;
	}

	if(polygonCount < 3){
		stringstream s;
		s << "Not enough indices specified for the face on line " << mLine << ".";
		throw s.str().c_str();
	}

	// triangulate if necessary //
	triangulateOBJFace(verts, norms, texCoords, triVerts, triNorms, triTexCoords, triMaterialIds);
}


/**
* \brief Triangulate a polygon and return the triangles
* 
* \param verts - the indices of the vertices
* \param norms - the indices of the normals
* \param texCoords - the indices of the texCoords
* \param triVerts - the returned indices of the triangle vertices
* \param triNorms - the returned indices of the triangle normals
* \param triTexCoords - the returned indices of the triangle texture coordinates
*/
void ModelParser::triangulateOBJFace(const vector<int>& verts, 
									 const vector<int>& norms, 
									 const vector<int>& texCoords,
									 vector<int3>& triVerts, 
									 vector<int3>& triNorms, 
									 vector<int3>& triTexCoords, 
									 vector<int>& triMaterialIds)
{
	unsigned int count = verts.size();

	if((norms.size()!=0) && (norms.size() != count))
		throw "Number of normals compared to vertices is incorrect.";
	if((texCoords.size()!=0) && (texCoords.size() != count))
		throw "Number of texCoords compared to vertices is incorrect.";


	for(unsigned int i=1; i<count-1; ++i){
		// first vertex is always included //
		// -1 because OBJ indices start with 1 //
		int3 v;
		v.data[0] = verts.at(0)  -1;
		v.data[1] = verts.at(i)  -1;
		v.data[2] = verts.at(i+1)-1;

		triVerts.push_back(v);

		if(norms.size()>0){
			int3 n;
			n.data[0] = norms.at(0)  -1;
			n.data[1] = norms.at(i)  -1;
			n.data[2] = norms.at(i+1)-1;
			triNorms.push_back(n);
		}

		if(texCoords.size()>0){
			int3 t;
			t.data[0] = texCoords.at(0)  -1;
			t.data[1] = texCoords.at(i)  -1;
			t.data[2] = texCoords.at(i+1)-1;

			triTexCoords.push_back(t);
		}

		triMaterialIds.push_back(mCurrentMaterial);
	}
}


/**
* \brief Converts the 
*/
void ModelParser::convertOBJinfos(const vector<vec3>& verts, 
								  const vector<vec3>& norms, 
								  const vector<vec2>& texCoords, 
								  const vector<int3>& triVerts, 
								  const vector<int3>& triNorms, 
								  const vector<int3>& triTexCoords,
								  vector<int>& triMaterialIds)
{
	// clear of old data //
	mTriangles.clear();
	mVertexNormals.clear();
	mTriangleNormals.clear();
	mTextureCoordinates.clear();
	mMaterialIds.clear();
	mTriangles.clear();
	if(norms.size() > 0){ mUseVertexNormals = true; }
	if(texCoords.size() > 0){ mHasTextureOrMaterial = true; }
	if(triTexCoords.size() > 0){ mHasTextureOrMaterial = true; }


	if(triNorms.size() == 0){
		// we just need to copy the arrays //
		mVertices = verts;
		mTextureCoordinates = texCoords;
		mTriangles = triVerts;
		
		for(unsigned int i=0; i < mTriangles.size(); ++i)
		{
			mMaterialIds.insert(std::pair<int,int>(i, triMaterialIds.at(i)));
		}
		return;
	}

	for(unsigned int i=0; i < triVerts.size(); ++i){
		assert(i < triVerts.size());
		int3 vidx = triVerts.at(i); // -1 because OBJ indices start at 1
		int3 nidx = (i < triNorms.size()) ? triNorms.at(i) : int3(0,0,0);
		int3 tidx = (i < triTexCoords.size()) ? triTexCoords.at(i) : int3(0,0,0);

		vec3 v[3];
		assert((unsigned long)(vidx.data[0]) < verts.size());
		assert((unsigned long)(vidx.data[1]) < verts.size());
		assert((unsigned long)(vidx.data[2]) < verts.size());
		v[0] = verts.at(vidx.data[0]);
		v[1] = verts.at(vidx.data[1]);
		v[2] = verts.at(vidx.data[2]);

		vec3 n[3];
		assert((unsigned long)(nidx.data[0]) < norms.size());
		assert((unsigned long)(nidx.data[1]) < norms.size());
		assert((unsigned long)(nidx.data[2]) < norms.size());
		n[0] = norms.at(nidx.data[0]);
		n[1] = norms.at(nidx.data[1]);
		n[2] = norms.at(nidx.data[2]);

		vec3 t[3];
		t[0] = ((unsigned long)(tidx.data[0]) < texCoords.size()) ? texCoords.at(tidx.data[0]) : 0.0f;
		t[1] = ((unsigned long)(tidx.data[1]) < texCoords.size()) ? texCoords.at(tidx.data[1]) : 0.0f;
		t[2] = ((unsigned long)(tidx.data[2]) < texCoords.size()) ? texCoords.at(tidx.data[2]) : 0.0f;

		int index = mVertices.size();

		for(int j=0; j<3; ++j){
			mVertices.push_back(v[j]);
			mVertexNormals.push_back(n[j]);
			mTextureCoordinates.push_back(t[j]);
		}

		int3 triangle;
		triangle.data[0] = index;
		triangle.data[1] = index+1;
		triangle.data[2] = index+2;

		mTriangles.push_back(triangle);

		assert(i < triMaterialIds.size());
		mMaterialIds.insert(std::pair<int,int>(mTriangles.size()-1, triMaterialIds.at(i)));
	}
}
