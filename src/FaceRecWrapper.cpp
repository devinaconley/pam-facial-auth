// =======================================================================
// pam-facial-auth
// FaceRecWrapper
//
// Created by Devin Conley
// =======================================================================

#include "FaceRecWrapper.h"

FaceRecWrapper::FaceRecWrapper() :
	height( 0 ),
	width( 0 )
{}

FaceRecWrapper::FaceRecWrapper( const std::string & techniqueName ) :
	height( 0 ),
	width( 0 )
{
	SetTechnique( techniqueName );
}

void FaceRecWrapper::Train( const std::vector<cv::Mat> & images, const std::vector<int> & labels )
{
	if ( !images.size() )
	{
		printf( "Empty vector of training images\n" );
		return;
	}

	height = images[0].rows;
	width  = images[0].cols;

	fr->train( images, labels );
}

void FaceRecWrapper::Predict( const cv::Mat & im, int & label, double & confidence )
{
	// Resize properly
	cv::Rect roi;
	double   ratioH = double( im.rows ) / double( height );
	double   ratioW = double( im.cols ) / double( width );

	// Iterate across various scales
	double       bestConfidence = 10000;
	int          bestLabel      = -1;
	for ( double scale          = 0.7; scale <= 1.0; scale += 0.01 )
	{
		// First crop to get to the same aspect ratio (crop in the larger relative dimension)
		if ( ratioH < ratioW )
		{
			roi.height = double( im.rows ) * scale;
			roi.width  = roi.height * ( double( width ) / double( height ) );
			roi.y      = double( im.rows - roi.height ) / 2.0;
			roi.x      = double( im.cols - roi.width ) / 2.0;
		}
		else
		{
			roi.width  = double( im.cols ) * scale;
			roi.height = roi.width * ( double( height ) / double( width ) );
			roi.x      = double( im.cols - roi.width ) / 2.0;
			roi.y      = double( im.rows - roi.height ) / 2.0;
		}
		// Then do crop and resize
		cv::Size sz;
		sz.height = height;
		sz.width  = width;
		cv::Mat imNew = im( roi );
		cv::resize( imNew, imNew, sz );

		// Do prediction
		double tempConfidence;
		int    tempLabel;
		fr->predict( imNew, tempLabel, tempConfidence );

		if ( tempConfidence > bestConfidence || bestLabel == -1 )
		{
			bestConfidence = tempConfidence;
			bestLabel      = tempLabel;
		}
	}
	// Set best scale as results
	confidence                  = bestConfidence;
	label                       = bestLabel;

}

void FaceRecWrapper::Save( const std::string & filename )
{
	std::string rawModelFile = filename + "-raw.xml";

	// Write additional wrapper info
	FILE * pModel;
	pModel = fopen( filename.c_str(), "w" );
	fprintf( pModel, "technique=%s\n", technique.c_str() );
	fprintf( pModel, "imageHeight=%d\n", (int) height );
	fprintf( pModel, "imageWidth=%d\n", (int) width );
	fprintf( pModel, "rawModelFile=%s\n", rawModelFile.c_str() ); // point at raw
	fclose( pModel );

	// Save actual model
	fr->save( rawModelFile );
}

void FaceRecWrapper::Load( const std::string & filename )
{
	std::map<std::string, std::string> model;
	Utils::GetConfig( filename, model );

	height = std::stoi( model["imageHeight"] );
	width  = std::stoi( model["imageWidth"] );
	SetTechnique( model["technique"] );

	fr->load( filename + "-raw.xml" );
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