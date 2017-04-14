// =======================================================================
// pam-facial-auth
// FacialAuth
//
// Created by Devin Conley
// =======================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <map>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <ctime>

#include "Utils.h"

/* expected hook */
PAM_EXTERN int pam_sm_setcred( pam_handle_t * pamh, int flags, int argc, const char ** argv )
{
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt( pam_handle_t * pamh, int flags, int argc, const char ** argv )
{
	return PAM_SUCCESS;
}

/* expected hook, this is where custom stuff happens */
PAM_EXTERN int pam_sm_authenticate( pam_handle_t * pamh, int flags, int argc, const char ** argv )
{
	// First verify we can get username
	const char * user;
	int ret = pam_get_user( pamh, &user, "Username: " );
	if ( ret != PAM_SUCCESS )
	{
		return ret;
	}
	std::string username( user );

	// Get config
	std::map<std::string, std::string> config;
	config["imageDir"]    = "/var/lib/motioneye/Camera1"; // defaults
	config["imageWindow"] = "5";

	Utils::GetConfig( "/etc/pam-facial-auth/config", config );

	// Parse number values
	std::size_t height    = std::stoi( config["imageHeight"] );
	std::size_t width     = std::stoi( config["imageWidth"] );
	std::time_t timeout   = std::stoi( config["timeout"] );
	double      threshold = std::stod( config["threshold"] );

	// Load model
	cv::Ptr<cv::face::FaceRecognizer> fr;
	fr = cv::face::createEigenFaceRecognizer( 10 ); // TODO : load correct type of model
	fr->load( "/etc/pam-facial-auth/model.xml" );

	// Loop control
	std::time_t start = std::time( NULL );
	std::string imagePathLast;

	while ( std::time( NULL ) - start < timeout )
	{
		// Find most recent image
		std::string imagePath = config["imageDir"];
		std::string temp;

		// Walk subdirectories
		std::vector<std::string> dates;
		std::vector<std::string> nullVec;
		Utils::WalkDirectory( config["imageDir"], nullVec, dates );

		// Get most recent daily folder, assuming named under yyyy-MM-dd
		for ( std::vector<std::string>::iterator it = dates.begin(); it != dates.end(); ++it )
		{
			if ( *it > temp )
			{
				temp = *it;
			}
		}
		imagePath += "/" + temp;

		// Walk image files
		std::vector<std::string> times;
		Utils::WalkDirectory( imagePath, times, nullVec );

		temp = "";
		// Get most recent image, assuming named under HH-mm-ss.ext
		for ( std::vector<std::string>::iterator it = times.begin(); it != times.end(); ++it )
		{
			if ( *it > temp )
			{
				temp = *it;
			}
		}
		imagePath += "/" + temp;

		// Check if new image
		if ( imagePath == imagePathLast )
		{
			continue;
		}

		cv::Mat im      = cv::imread( imagePath, CV_LOAD_IMAGE_GRAYSCALE );
		if ( !im.size().area() > 0 )
		{
			continue;
		}

		std::cout << imagePath << std::endl;

		// Resize properly
		cv::Rect roi;
		double   ratioH = double( im.rows ) / double( height );
		double   ratioW = double( im.cols ) / double( width );
		// First crop to get to the same aspect ratio (crop in the larger relative dimension)
		if ( ratioH < ratioW )
		{
			roi.height = im.rows;
			roi.width  = double( im.rows ) * ( double( width ) / double( height ) );
			roi.y      = 0.0;
			roi.x      = double( im.cols - roi.width ) / 2.0;
		}
		else
		{
			roi.width  = im.cols;
			roi.height = double( im.cols ) * ( double( height ) / double( width ) );
			roi.x      = 0.0;
			roi.y      = double( im.rows - roi.height ) / 2.0;
		}
		// Then do crop and resize
		cv::Size sz;
		sz.height = height;
		sz.width  = width;
		im = im( roi );
		cv::resize( im, im, sz );

		// Do prediction
		double confidence = 0.0;
		int    prediction = -1;
		fr->predict( im, prediction, confidence );

		printf( "Predicted: %s, %d, %s (%f)\n",
			imagePath.c_str(), prediction, fr->getLabelInfo( prediction ).c_str(), confidence );

		if ( confidence < threshold && fr->getLabelInfo( prediction ) == username )
		{
			return PAM_SUCCESS;
		}
		imagePathLast = imagePath;
	}

	printf( "Timeout on face authentication... \n" );
	return PAM_AUTH_ERR;
}


