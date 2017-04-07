// =======================================================================
// pam-facial-auth
// run_training
//
// Created by Devin Conley
// =======================================================================

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <dirent.h>

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

	DIR           * dirMain;
	DIR           * dirSub;
	struct dirent * curr;

	dirMain = opendir( pathDir.c_str() );
	if ( dirMain )
	{
		// iterate through users
		curr = readdir( dirMain );
		while ( curr )
		{
			// verify valid subdirectory
			std::string user = curr->d_name;
			if ( user.find( "." ) == std::string::npos )
			{
				int label = usernames.size();

				dirSub = opendir( ( pathDir + "/" + user ).c_str() );

				// iterate through images of user
				curr = readdir( dirSub );
				while ( curr )
				{
					cv::Mat temp = cv::imread( pathDir + "/" + user + "/" + curr->d_name,
						CV_LOAD_IMAGE_GRAYSCALE );
					if ( temp.data )
					{
						images.push_back( temp );
						labels.push_back( label );
					}

					curr = readdir( dirSub );
				}
				usernames.push_back( user );
			}

			curr = readdir( dirMain );
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
	printf( "Training %s model...", technique.c_str() );
	fr->train( images, labels );

	// Test?
	for ( size_t i = 0; i < images.size(); ++i )
	{
		double confidence = 0.0;
		int    prediction = -1;
		fr->predict( images[i], prediction, confidence );
		printf( "Predicted: %d (%f), actual: %d\n", prediction, confidence, labels[i] );
	}

	// Set usernames
	for ( size_t i = 0; i < usernames.size(); ++i )
	{
		fr->setLabelInfo( i, usernames[i] );
	}

	// Write out
	fr->save( "trained_model.xml" );

	return 0;
}