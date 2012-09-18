#ifndef _OBJLOADER_H_
#define _OBJLOADER_H_
/*
 *	OBJLoader.h
 *
 *	An OBJ model loader.
 *
 *	Coded by Joseph A. Marrero. 12/22/06
 *	Modified 1/17/08
 *	http://www.ManVsCode.com/
 */
#include <fstream>
#include <string>
#include <vector>
#include <map>

class OBJLoader
{
  public:
	typedef float       Scalar;
	typedef std::string String;

	struct Vertex {
		Scalar x; Scalar y; Scalar z;
	};

	struct TextureCoord {
		Scalar u; Scalar v;
	};

	struct Normal {
		Scalar nx; Scalar ny; Scalar nz;
	};

	typedef std::vector<unsigned int> IndexCollection;	

	struct Face {
		IndexCollection vertexIndices; 
		IndexCollection textureIndices;
		IndexCollection normalIndices;
	};

	struct MeshVertex { // can be used with GL_T2F_N3F_V3F
		union {
			struct { Scalar u, v; };
			TextureCoord textureCoordinate;
		};
		union {
			struct { Scalar nx, ny, nz; };
			Normal normal;
		};
		union {
			struct { Scalar x, y, z; };
			Vertex vertex;
		};
	};

	struct MeshFace {
		IndexCollection indices;
	}; 

	typedef std::vector<OBJLoader::MeshVertex> Mesh;
	typedef std::vector<OBJLoader::MeshFace>   MeshFaceCollection;

	struct Group {
		String             groupName;
		Mesh               mesh;
		MeshFaceCollection faces;
	};

	typedef std::vector<OBJLoader::Vertex>       VertexCollection;
	typedef std::vector<OBJLoader::TextureCoord> TextureCoordCollection;
	typedef std::vector<OBJLoader::Normal>       NormalCollection;
	typedef std::vector<OBJLoader::Group>        GroupCollection;
	typedef std::vector<OBJLoader::Face>         FaceCollection;

  public:
	OBJLoader( );
	~OBJLoader( );

	bool load( const char *filename, bool verbose = false );
	void clear( );
	GroupCollection&              mesh( );
	const GroupCollection&        mesh( ) const;
	VertexCollection&             vertices( );
	const VertexCollection&       vertices( ) const;
	TextureCoordCollection&       textureCoordinates( );
	const TextureCoordCollection& textureCoordinates( ) const;
	NormalCollection&             normals( );
	const NormalCollection&       normals( ) const;
	FaceCollection&               faces( );
	const FaceCollection&         faces( ) const;

  protected:
	enum Element {
		ELM_VERTEX,
		ELM_TEXTURECOORD,
		ELM_NORMAL,
		ELM_FACE,
		ELM_GROUP,
		ELM_OTHER
	};

	void addDefaultGroup( );
	void setElement( char *token );
	void addVertex( );
	void addTextureCoord( );
	void addNormal( );
	void addFace( );
	void addGroup( );

	std::ifstream          m_File;
	Element                m_CurrentElement;
	bool                   m_bVerboseOutput;
	String                 m_CurrentGroup;
	VertexCollection       m_Vertices;
	TextureCoordCollection m_TextureCoords;
	NormalCollection       m_Normals;
	GroupCollection        m_Groups;
	FaceCollection         m_Faces;
};

inline OBJLoader::GroupCollection &OBJLoader::mesh( )
{ return m_Groups; }

inline const OBJLoader::GroupCollection &OBJLoader::mesh( ) const
{ return m_Groups; }

inline OBJLoader::VertexCollection &OBJLoader::vertices( )
{ return m_Vertices; }

inline const OBJLoader::VertexCollection &OBJLoader::vertices( ) const
{ return m_Vertices; }

inline OBJLoader::TextureCoordCollection &OBJLoader::textureCoordinates( )
{ return m_TextureCoords; }

inline const OBJLoader::TextureCoordCollection &OBJLoader::textureCoordinates( ) const
{ return m_TextureCoords; }

inline OBJLoader::NormalCollection &OBJLoader::normals( )
{ return m_Normals; }

inline const OBJLoader::NormalCollection &OBJLoader::normals( ) const
{ return m_Normals; }

inline OBJLoader::FaceCollection &OBJLoader::faces( )
{ return m_Faces; }

inline const OBJLoader::FaceCollection &OBJLoader::faces( ) const
{ return m_Faces; }

#endif // _OBJLOADER_H_
