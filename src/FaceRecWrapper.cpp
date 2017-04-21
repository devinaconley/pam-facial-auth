// =======================================================================
// pam-facial-auth
// FaceRecWrapper
//
// Created by Devin Conley
// =======================================================================

#include "FaceRecWrapper.h"

FaceRecWrapper::FaceRecWrapper() :
	sizeFace( 96 )
{}

FaceRecWrapper::FaceRecWrapper( const std::string & techniqueName, const std::string & pathCascade ) :
	sizeFace( 96 )
{
	SetTechnique( techniqueName );
	LoadCascade( pathCascade );
}

void FaceRecWrapper::Train( const std::vector<cv::Mat> & images, const std::vector<int> & labels )
{
	if ( !images.size() )
	{
		printf( "Empty vector of training images\n" );
		return;
	}

	std::vector<cv::Mat> imagesCropped;
	std::vector<int>     labelsCropped;

	for ( size_t i = 0; i < images.size(); ++i )
	{
		cv::Mat crop;
		if ( CropFace( images[i], crop ) )
		{
			labelsCropped.push_back( labels[i] );
			imagesCropped.push_back( crop );
		}
	}

	fr->train( imagesCropped, labelsCropped );
}

void FaceRecWrapper::Predict( const cv::Mat & im, int & label, double & confidence )
{
	cv::Mat cropped;
	if ( !CropFace( im, cropped ) )
	{
		label      = -1;
		confidence = 10000;
		return;
	}

	fr->predict( cropped, label, confidence );
}

void FaceRecWrapper::Save( const std::string & path )
{
	// Write additional wrapper info
	FILE * pModel;
	pModel = fopen( path.c_str(), "w" );
	fprintf( pModel, "technique=%s\n", technique.c_str() );
	fprintf( pModel, "sizeFace=%d\n", (int) sizeFace );
	fclose( pModel );

	// Save actual model and classifier
	fr->save( path + "-facerec.xml" );
	std::ifstream orig( pathCascade, std::ios::binary );
	std::ofstream cpy( path + "-cascade.xml", std::ios::binary );
	cpy << orig.rdbuf();
}

void FaceRecWrapper::Load( const std::string & path )
{
	std::map<std::string, std::string> model;
	Utils::GetConfig( path, model );

	sizeFace = std::stoi( model["sizeFace"] );
	SetTechnique( model["technique"] );

	LoadCascade( path + "-cascade.xml" );
	fr->load( path + "-facerec.xml" );
}

void FaceRecWrapper::SetLabelNames( const std::vector<std::string> & names )
{
	for ( std::size_t i = 0; i < names.size(); ++i )
	{
		fr->setLabelInfo( i, names[i] );
	}
}

std::string FaceRecWrapper::GetLabelName( int index )
{
	return fr->getLabelInfo( index );
}

bool FaceRecWrapper::SetTechnique( const std::string & t )
{
	if ( t == "eigen" )
	{
		fr = cv::face::createEigenFaceRecognizer( 10 );
	}
	else if ( t == "fisher" )
	{
		fr = cv::face::createFisherFaceRecognizer();
	}
	else if ( t == "lbph" )
	{
		fr = cv::face::createLBPHFaceRecognizer();
	}
	else
	{
		printf( "Invalid technique: %s, defaulting to eigenfaces.\n", technique.c_str() );
		fr        = cv::face::createEigenFaceRecognizer( 10 );
		technique = "eigen";
		return false;
	}

	technique = t;
	return true;
}

bool FaceRecWrapper::LoadCascade( const std::string & filepath )
{
	pathCascade = filepath;

	// Set up face detector
	if ( !cascade.load( pathCascade ) )
	{
		printf( "Could not load haar cascade classifier.\n" );
		return false;
	}
	return true;
}

bool FaceRecWrapper::CropFace( const cv::Mat & image, cv::Mat & cropped )
{
	// Detect face
	std::vector<cv::Rect> faces;
	cascade.detectMultiScale( image, faces, 1.05, 8, 0 | CV_HAAR_SCALE_IMAGE, cv::Size( 40, 40 ) );
	if ( !faces.size() )
	{
		return false;
	}

	// Crop and resize
	cropped = image( faces[0] );
	cv::resize( cropped, cropped, cv::Size( sizeFace, sizeFace ) );

	return true;
}