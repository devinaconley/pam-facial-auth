// =======================================================================
// pam-facial-auth
// run_training
//
// Created by Devin Conley
// =======================================================================

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <dirent.h>

int main( int argc, char ** argv )
{
	if ( argc != 2 )
	{
		printf( "usage: run_training <path to image directory>\n"
			"--(Note: should be organized into subdirectories by username)\n" );
		return -1;
	}

	std::string pathDir = argv[1];

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
					cv::Mat temp = cv::imread( pathDir + "/" + user + "/" + curr->d_name );
					if ( temp.data )
					{
						images.push_back( temp );
						labels.push_back( label );
						printf( "%s\n", curr->d_name );
					}

					curr = readdir( dirSub );
				}
				usernames.push_back( user );
			}

			curr = readdir( dirMain );
		}
	}

	if ( images.size() )
	{
		cv::namedWindow( "Display Image", cv::WINDOW_AUTOSIZE );
		cv::imshow( "Display Image", images[0] );
		cv::waitKey( 0 );
	}
	return 0;
}