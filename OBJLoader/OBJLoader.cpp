/*
 *	OBJLoader.cpp
 *
 *	An OBJ model loader.
 *
 *	Coded by Joseph A. Marrero. 12/22/06
 *	http://www.l33tprogramer.com/
 */
#include <iostream>
#include <sstream>
#include <cassert>
#include "OBJLoader.h"

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
OBJLoader::OBJLoader( )
  : m_File(),
    m_CurrentElement(ELM_OTHER), 
    m_bVerboseOutput(false),
	m_CurrentGroup("")
{
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
OBJLoader::~OBJLoader( )
{
	m_File.close( );
}
///////////////////////////////////////////////////////////////////////////////////
/// Load a OBJ model file./////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// verbose: output verbose information during reading. ////////////////////////////
// storeFaceInfo: Should we optimize for a known face format such as  /////////////
//   triangles. ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
bool OBJLoader::load( const char *filename, bool verbose )
{
	assert( filename != NULL && *filename != NULL ); // must be a valid filename
	m_File.open( filename, std::ios::in );
	m_bVerboseOutput = verbose;

	String line("");

	if( m_File.fail( ) ) 
		return false;

	// read in the file...
	while( !m_File.eof( ) )
	{
		getline( m_File, line );
		if( line.empty( ) ) continue; // avoid empty lines...

		char *token = strtok( const_cast<char *>( line.c_str() ), " " );
		setElement( token );


		switch( m_CurrentElement )
		{
			case ELM_VERTEX:
				addVertex( );
				break;
			case ELM_NORMAL:
				addNormal( );
				break;
			case ELM_TEXTURECOORD:
				addTextureCoord( );
				break;
			case ELM_FACE:
				addFace( );
				break;
			case ELM_GROUP:
				addGroup( );
				break;
			case ELM_OTHER:
			default:
				if( m_bVerboseOutput )
					std::cout << "[OBJLoader] Unknown element; Skipping line...\n\t" << line << std::endl;
				continue;
				break;
		}
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void OBJLoader::clear( )
{
	m_Groups.clear( );
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void OBJLoader::setElement( char *token )
{

	if( strncmp( token, "vt", 2 ) == 0 )
		m_CurrentElement = ELM_TEXTURECOORD;
	else if( strncmp( token, "vn", 2 ) == 0 )
		m_CurrentElement = ELM_NORMAL;
	else if( strncmp( token, "v", 1 ) == 0 )
		m_CurrentElement = ELM_VERTEX;
	else if( strncmp( token, "f", 1 ) == 0 )
		m_CurrentElement = ELM_FACE;
	else if( strncmp( token, "g", 1 ) == 0 )
		m_CurrentElement = ELM_GROUP;
	else
		m_CurrentElement = ELM_OTHER;
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void OBJLoader::addVertex( )
{
	Vertex v;

	char *token = strtok( NULL, " " );
	v.x = static_cast<Scalar>( atof( token ) );

	token = strtok( NULL, " " );
	v.y = static_cast<Scalar>( atof( token ) );

	token = strtok( NULL, " " );
	v.z = static_cast<Scalar>( atof( token ) );

	m_Vertices.push_back( v );
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void OBJLoader::addTextureCoord( )
{
	TextureCoord vt;

	char *token = strtok( NULL, " " );
	vt.u = static_cast<Scalar>( atof( token ) );

	token = strtok( NULL, " " );
	vt.v = static_cast<Scalar>( atof( token ) );

	m_TextureCoords.push_back( vt );
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void OBJLoader::addNormal( )
{
	Normal n;

	char *token = strtok( NULL, " " );
	n.nx = static_cast<Scalar>( atof( token ) );

	token = strtok( NULL, " " );
	n.ny = static_cast<Scalar>( atof( token ) );

	token = strtok( NULL, " " );
	n.nz = static_cast<Scalar>( atof( token ) );

	m_Normals.push_back( n );
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void OBJLoader::addFace( )
{
	char *token = strtok( NULL, " " );
	unsigned int vIndex = 0;
	unsigned int tIndex = 0;
	unsigned int nIndex = 0;

	if( m_CurrentGroup.compare("") == 0 ) // check if there are no groups;
	{
		if( m_bVerboseOutput )
			std::cout << "[OBJLoader] No group defined yet, so I am adding a default group." << std::endl;
		addDefaultGroup( ); // if none create a default one...
	}

	Group *group = NULL;

	// Can we slow if there are lots of groups, but it simplifies the
	// code.
	for( unsigned int i = 0; i < m_Groups.size( ); i++ )
	{
		if( m_Groups[ i ].groupName == m_CurrentGroup )
		{
			group = &m_Groups[ i ];
			break;
		}
	}

	assert( group != NULL ); // could not find group

	
	MeshFace meshFace;
	Face face;

	while( token != NULL )
	{
		char garbage;
		String tok( token );
		std::istringstream iss( tok );

		iss >> vIndex  >> garbage >> tIndex >> garbage >> nIndex;

		// Store the vertex, normal, and UVs into a MeshVertex...
		// We subtract 1 because the OBJ format counts from 1 (instead of 0)
		MeshVertex v;
		v.x  = m_Vertices[ vIndex - 1 ].x;
		v.y  = m_Vertices[ vIndex - 1 ].y;
		v.z  = m_Vertices[ vIndex - 1 ].z;
		v.u  = m_TextureCoords[ tIndex - 1 ].u;
		v.v  = m_TextureCoords[ tIndex - 1 ].v;
		v.nx = m_Normals[ nIndex - 1 ].nx;
		v.ny = m_Normals[ nIndex - 1 ].ny;
		v.nz = m_Normals[ nIndex - 1 ].nz;

		group->mesh.push_back( v );
		token = strtok( NULL, " " );

		face.vertexIndices.push_back( vIndex - 1 );
		face.textureIndices.push_back( tIndex - 1 );
		face.normalIndices.push_back( nIndex - 1 );

		meshFace.indices.push_back( group->mesh.size( ) - 1 );
	}

	m_Faces.push_back( face );
	group->faces.push_back( meshFace );
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void OBJLoader::addGroup( )
{
	String groupName;
	char *token = strtok( NULL, " " );

	// concatenate all tokens to form group name.
	while( token != NULL )
	{
		groupName += token;
		groupName += " ";
		token = strtok( NULL, " " );
	}

	groupName = groupName.substr( 0, groupName.length() - 1 ); // remove last space char

	// Create a new group and add it...
	Group g;
	g.groupName = groupName;
	m_Groups.push_back( g );

	m_CurrentGroup = groupName;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void OBJLoader::addDefaultGroup( )
{
	// Create a new group and add it...
	Group g;
	g.groupName = "default";
	m_Groups.push_back( g );

	m_CurrentGroup = g.groupName;
}

