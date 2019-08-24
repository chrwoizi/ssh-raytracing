/*
	based on ply2obj from libply. http://www.cs.kuleuven.ac.be/~ares/libply/
*/


#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include <tr1/functional>

#include <ply.hpp>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifndef _int3_def
#define _int3_def
struct int3{
	int data[3];
};
#endif

using namespace std::tr1::placeholders;

class PlyloaderWrapper
{
	public:
		typedef int flags_type;
		enum { triangulate = 1 << 0 };
		PlyloaderWrapper(flags_type flags = 0);
		bool load(std::istream& istream, const std::string& istream_filename, std::vector<vec>& vertices, std::vector<vec>& normals, std::vector<int3>& faces);
		unsigned long getFaceCount(std::istream& istream, const std::string& istream_filename);
	private:
		std::vector<vec>* vertices;
		std::vector<vec>* normals;
		std::vector<int3>* faces;
		bool hasNormal;
		bool onlyFaceCount;
		unsigned long faceCount;
		
		void info_callback(const std::string& filename, std::size_t line_number, const std::string& message);
		void warning_callback(const std::string& filename, std::size_t line_number, const std::string& message);
		void error_callback(const std::string& filename, std::size_t line_number, const std::string& message);
		std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> > element_definition_callback(const std::string& element_name, std::size_t count);
		template <typename ScalarType> std::tr1::function<void (ScalarType)> scalar_property_definition_callback(const std::string& element_name, const std::string& property_name);
		template <typename SizeType, typename ScalarType> std::tr1::tuple<std::tr1::function<void (SizeType)>, std::tr1::function<void (ScalarType)>, std::tr1::function<void ()> > list_property_definition_callback(const std::string& element_name, const std::string& property_name);
		void vertex_begin();
		void vertex_x(ply::float32 x);
		void vertex_y(ply::float32 y);
		void vertex_z(ply::float32 z);
		void vertex_end();
		void normal_x(ply::float32 x);
		void normal_y(ply::float32 y);
		void normal_z(ply::float32 z);
		void face_begin();
		void face_vertex_indices_begin(ply::uint8 size);
		void face_vertex_indices_element(ply::int32 vertex_index);
		void face_vertex_indices_end();
		void face_end();
		flags_type flags_;
		double vertex_x_, vertex_y_, vertex_z_;
		double normal_x_, normal_y_, normal_z_;
		std::size_t face_vertex_indices_element_index_, face_vertex_indices_first_element_, face_vertex_indices_previous_element_;
};

PlyloaderWrapper::PlyloaderWrapper(flags_type flags)
	: flags_(flags)
{
}

void PlyloaderWrapper::info_callback(const std::string& filename, std::size_t line_number, const std::string& message)
{
	//std::cerr << filename << ":" << line_number << ": " << "info: " << message << std::endl;
}

void PlyloaderWrapper::warning_callback(const std::string& filename, std::size_t line_number, const std::string& message)
{
	//std::cerr << filename << ":" << line_number << ": " << "warning: " << message << std::endl;
}

void PlyloaderWrapper::error_callback(const std::string& filename, std::size_t line_number, const std::string& message)
{
	std::cerr << filename << ":" << line_number << ": " << "error: " << message << std::endl;
}

std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> > PlyloaderWrapper::element_definition_callback(const std::string& element_name, std::size_t count)
{
	if (element_name == "vertex") {
		if(onlyFaceCount) return std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> >(0, 0);
		return std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> >(
				std::tr1::bind(&PlyloaderWrapper::vertex_begin, this),
				std::tr1::bind(&PlyloaderWrapper::vertex_end, this)
										);
	}
	else if (element_name == "face") {
		return std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> >(
				std::tr1::bind(&PlyloaderWrapper::face_begin, this),
				std::tr1::bind(&PlyloaderWrapper::face_end, this)
										);
	}
	else {
		return std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> >(0, 0);
	}
}

template <>
std::tr1::function<void (ply::float32)> PlyloaderWrapper::scalar_property_definition_callback(const std::string& element_name, const std::string& property_name)
{
	if(onlyFaceCount) return 0;
	if (element_name == "vertex") {
		if (property_name == "x") {
			return std::tr1::bind(&PlyloaderWrapper::vertex_x, this, _1);
		}
		else if (property_name == "y") {
			return std::tr1::bind(&PlyloaderWrapper::vertex_y, this, _1);
		}
		else if (property_name == "z") {
			return std::tr1::bind(&PlyloaderWrapper::vertex_z, this, _1);
		}
		if (property_name == "nx") {
			return std::tr1::bind(&PlyloaderWrapper::normal_x, this, _1);
		}
		else if (property_name == "ny") {
			return std::tr1::bind(&PlyloaderWrapper::normal_y, this, _1);
		}
		else if (property_name == "nz") {
			return std::tr1::bind(&PlyloaderWrapper::normal_z, this, _1);
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}
}

template <>
std::tr1::tuple<std::tr1::function<void (ply::uint8)>, std::tr1::function<void (ply::int32)>, std::tr1::function<void ()> > PlyloaderWrapper::list_property_definition_callback(const std::string& element_name, const std::string& property_name)
{
	if ((element_name == "face") && (property_name == "vertex_indices")) {
		return std::tr1::tuple<std::tr1::function<void (ply::uint8)>, std::tr1::function<void (ply::int32)>, std::tr1::function<void ()> >(
				std::tr1::bind(&PlyloaderWrapper::face_vertex_indices_begin, this, _1),
				std::tr1::bind(&PlyloaderWrapper::face_vertex_indices_element, this, _1),
						std::tr1::bind(&PlyloaderWrapper::face_vertex_indices_end, this)
										);
	}
	else {
		return std::tr1::tuple<std::tr1::function<void (ply::uint8)>, std::tr1::function<void (ply::int32)>, std::tr1::function<void ()> >(0, 0, 0);
	}
}

void PlyloaderWrapper::vertex_begin()
{
	hasNormal = false;
}

void PlyloaderWrapper::vertex_x(ply::float32 x)
{
	vertex_x_ = x;
}

void PlyloaderWrapper::vertex_y(ply::float32 y)
{
	vertex_y_ = y;
}

void PlyloaderWrapper::vertex_z(ply::float32 z)
{
	vertex_z_ = z;
}

void PlyloaderWrapper::vertex_end()
{
	vertices->push_back(vec(vertex_x_, vertex_y_, vertex_z_));
	if(hasNormal)
	{
		normals->push_back(vec(normal_x_, normal_y_, normal_z_));
	}
	//(*ostream_) << "v " << vertex_x_ << " " << vertex_y_ << " " << vertex_z_ << "\n";
}

void PlyloaderWrapper::normal_x(ply::float32 x)
{
	hasNormal = true;
	normal_x_ = x;
}

void PlyloaderWrapper::normal_y(ply::float32 y)
{
	hasNormal = true;
	normal_y_ = y;
}

void PlyloaderWrapper::normal_z(ply::float32 z)
{
	hasNormal = true;
	normal_z_ = z;
}

void PlyloaderWrapper::face_begin()
{
	if(onlyFaceCount) faceCount++;
}

void PlyloaderWrapper::face_vertex_indices_begin(ply::uint8 size)
{
	face_vertex_indices_element_index_ = 0;
}

void PlyloaderWrapper::face_vertex_indices_element(ply::int32 vertex_index)
{
	if(onlyFaceCount) return;
	if (flags_ & triangulate) {
		if (face_vertex_indices_element_index_ == 0) {
			face_vertex_indices_first_element_ = vertex_index;
		}
		else if (face_vertex_indices_element_index_ == 1) {
			face_vertex_indices_previous_element_ = vertex_index;
		}
		else {
			int3 face;
			face.data[0] = face_vertex_indices_first_element_;
			face.data[1] = face_vertex_indices_previous_element_;
			face.data[2] = vertex_index;
			faces->push_back(face);
			face_vertex_indices_previous_element_ = vertex_index;
		}
		++face_vertex_indices_element_index_;
	}
}

void PlyloaderWrapper::face_vertex_indices_end()
{
	if (!(flags_ & triangulate)) {
		//(*ostream_) << "\n";
	}
}

void PlyloaderWrapper::face_end()
{
}

bool PlyloaderWrapper::load(std::istream& istream, const std::string& istream_filename, std::vector<vec>& vertices, std::vector<vec>& normals, std::vector<int3>& faces)
{
	onlyFaceCount = false;
	this->vertices = &vertices;
	this->normals = &normals;
	this->faces = &faces;
	
	ply::ply_parser::flags_type ply_parser_flags = 0;
	ply::ply_parser ply_parser(ply_parser_flags);

	ply_parser.info_callback(std::tr1::bind(&PlyloaderWrapper::info_callback, this, std::tr1::ref(istream_filename), _1, _2));
	ply_parser.warning_callback(std::tr1::bind(&PlyloaderWrapper::warning_callback, this, std::tr1::ref(istream_filename), _1, _2));
	ply_parser.error_callback(std::tr1::bind(&PlyloaderWrapper::error_callback, this, std::tr1::ref(istream_filename), _1, _2));

	ply_parser.element_definition_callback(std::tr1::bind(&PlyloaderWrapper::element_definition_callback, this, _1, _2));

	ply::ply_parser::scalar_property_definition_callbacks_type scalar_property_definition_callbacks;
	ply::at<ply::float32>(scalar_property_definition_callbacks) = std::tr1::bind(&PlyloaderWrapper::scalar_property_definition_callback<ply::float32>, this, _1, _2);
	ply_parser.scalar_property_definition_callbacks(scalar_property_definition_callbacks);

	ply::ply_parser::list_property_definition_callbacks_type list_property_definition_callbacks;
	ply::at<ply::uint8, ply::int32>(list_property_definition_callbacks) = std::tr1::bind(&PlyloaderWrapper::list_property_definition_callback<ply::uint8, ply::int32>, this, _1, _2);
	ply_parser.list_property_definition_callbacks(list_property_definition_callbacks);

	return ply_parser.parse(istream);
}

unsigned long PlyloaderWrapper::getFaceCount(std::istream& istream, const std::string& istream_filename)
{
	onlyFaceCount = true;
	faceCount = 0;
	
	ply::ply_parser::flags_type ply_parser_flags = 0;
	ply::ply_parser ply_parser(ply_parser_flags);

	ply_parser.info_callback(std::tr1::bind(&PlyloaderWrapper::info_callback, this, std::tr1::ref(istream_filename), _1, _2));
	ply_parser.warning_callback(std::tr1::bind(&PlyloaderWrapper::warning_callback, this, std::tr1::ref(istream_filename), _1, _2));
	ply_parser.error_callback(std::tr1::bind(&PlyloaderWrapper::error_callback, this, std::tr1::ref(istream_filename), _1, _2));

	ply_parser.element_definition_callback(std::tr1::bind(&PlyloaderWrapper::element_definition_callback, this, _1, _2));

	ply::ply_parser::scalar_property_definition_callbacks_type scalar_property_definition_callbacks;
	ply::at<ply::float32>(scalar_property_definition_callbacks) = std::tr1::bind(&PlyloaderWrapper::scalar_property_definition_callback<ply::float32>, this, _1, _2);
	ply_parser.scalar_property_definition_callbacks(scalar_property_definition_callbacks);

	ply::ply_parser::list_property_definition_callbacks_type list_property_definition_callbacks;
	ply::at<ply::uint8, ply::int32>(list_property_definition_callbacks) = std::tr1::bind(&PlyloaderWrapper::list_property_definition_callback<ply::uint8, ply::int32>, this, _1, _2);
	ply_parser.list_property_definition_callbacks(list_property_definition_callbacks);

	ply_parser.parse(istream);

	return faceCount;
}

bool loadPly(const char* istream_filename, std::vector<vec>& vertices, std::vector<vec>& normals, std::vector<int3>& faces)
{
	std::ifstream ifstream;
	ifstream.open(istream_filename);

	class PlyloaderWrapper PlyloaderWrapper(PlyloaderWrapper::triangulate);
	return PlyloaderWrapper.load(ifstream, istream_filename, vertices, normals, faces);
}

unsigned long getFaceCountFromPly(const char* istream_filename)
{
	std::ifstream ifstream;
	ifstream.open(istream_filename);

	class PlyloaderWrapper PlyloaderWrapper(PlyloaderWrapper::triangulate);
	return PlyloaderWrapper.getFaceCount(ifstream, istream_filename);
}