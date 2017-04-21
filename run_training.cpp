// =======================================================================
// pam-facial-auth
// run_training
//
// Created by Devin Conley
// =======================================================================

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "src/Utils.h"
#include "src/FaceRecWrapper.h"

int main( int argc, char ** argv )
{
	if ( argc != 2 && argc != 3 )
	{
		printf( "usage: run_training <path to image directory> <facial recognition technique>\n"
			"--Note: directory should be organized into subdirectories by username\n"
			"--options: eigen (default), fisher, lbph\n" );
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

	for ( std::size_t i = 0; i < usernames.size(); ++i )
	{
		std::vector<std::string> files;
		Utils::WalkDirectory( pathDir + "/" + usernames[i], files, nullVec );
		for ( std::size_t j = 0; j < files.size(); ++j )
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
	FaceRecWrapper frw( technique, "etc/haarcascade_frontalface_default.xml" );

	// Do training
	printf( "Training %s model...\n", technique.c_str() );
	frw.Train( images, labels );

	// Set usernames
	frw.SetLabelNames( usernames );

	// Write out model
	frw.Save( "model" );

	// Write default config file
	FILE * pConfig;
	pConfig = fopen( "config", "w" );
	fprintf( pConfig, "imageDir=/var/lib/motioneye/Camera1\n" );
	fprintf( pConfig, "timeout=10\n" );
	fprintf( pConfig, "threshold=%.2f\n", 1000.0 );
	fclose( pConfig );

	printf( "Success. Config and model files written.\n" );
	return 0;
}