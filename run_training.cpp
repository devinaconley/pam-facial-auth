// =======================================================================
// pam-facial-auth
// run_training
//
// Created by Devin Conley
// =======================================================================

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include "src/Utils.h"

int main( int argc, char ** argv )
{
	if ( argc != 2 && argc != 3 )
	{
		printf( "usage: run_training <path to image directory> <facial recognition technique>\n"
			"--Note: directory should be organized into subdirectories by username\n"
			"--options: eigen (default), fisher, lbph" );
		return -1;
	}

	std::string pathDir   = argv[1];
	std::string technique = "eigen";

	if ( argc == 3 )
	{
		technique = argv[2];
	}

	// Collect all files in specified directory
	std::vector<std::string> usernames;
	std::vector<cv::Mat>     images;
	std::vector<int>         labels;

	std::vector<std::string> nullVec;

	Utils::WalkDirectory( pathDir, nullVec, usernames );

	for ( size_t i = 0; i < usernames.size(); ++i )
	{
		std::vector<std::string> files;
		Utils::WalkDirectory( pathDir + "/" + usernames[i], files, nullVec );
		for ( size_t j = 0; j < files.size(); ++j )
		{
			cv::Mat temp = cv::imread( pathDir + "/" + usernames[i] + "/" + files[j],
				CV_LOAD_IMAGE_GRAYSCALE );
			if ( temp.data )
			{
				images.push_back( temp );
				labels.push_back( i );
			}
		}
	}

	if ( !images.size() )
	{
		printf( "No images found in: %s\n", pathDir.c_str() );
		return -1;
	}

	// Select technique
	cv::Ptr<cv::face::FaceRecognizer> fr;

	if ( technique == "eigen" )
	{
		fr = cv::face::createEigenFaceRecognizer( 10 );
	}
	else if ( technique == "fisher" )
	{
		fr = cv::face::createFisherFaceRecognizer();
	}
	else if ( technique == "lbph" )
	{
		fr = cv::face::createLBPHFaceRecognizer();
	}
	else
	{
		printf( "Invalid technique: %s", technique.c_str() );
	}

	// Do training
	printf( "Training %s model...\n", technique.c_str() );
	fr->train( images, labels );

	// Set usernames
	for ( size_t i = 0; i < usernames.size(); ++i )
	{
		fr->setLabelInfo( i, usernames[i] );
	}

	// Write out
	fr->save( "trained_model.xml" );

	return 0;
}