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
	const char * username;
	int ret = pam_get_user( pamh, &username, "Username: " );
	if ( ret != PAM_SUCCESS )
	{
		return ret;
	}

	// Get config
	std::map<std::string, std::string> config;
	config["imageDir"]    = "/var/lib/motioneye/Camera1"; // defaults
	config["imageWindow"] = "5";

	Utils::GetConfig( "/etc/pam-facial-auth/config", config );

	// Parse number values
	size_t height      = std::stoi( config["imageHeight"] );
	size_t width       = std::stoi( config["imageWidth"] );
	time_t timeout     = std::stoi( config["timeout"] );
	size_t imageWindow = std::stoi( config["imageWindow"] );


	// Find recent images of interest
	std::vector<std::string> imagePaths;

	// Assume daily folders named under yyyy-MM-dd
	std::vector<std::string> dates;
	std::vector<std::string> nullVec;
	Utils::WalkDirectory( config["imageDir"], nullVec, dates );
	std::sort( dates.rbegin(), dates.rend() );

	for ( std::vector<std::string>::iterator itDate = dates.begin();
		  imagePaths.size() < imageWindow && itDate != dates.end(); ++itDate )
	{
		// Assume files named under HH-mm-ss.ext
		std::vector<std::string> times;
		Utils::WalkDirectory( config["imageDir"] + "/" + *itDate, times, nullVec );
		std::sort( times.rbegin(), times.rend() );

		for ( std::vector<std::string>::iterator itTime = times.begin();
			  imagePaths.size() < imageWindow && itTime != times.end(); ++itTime )
		{
			imagePaths.push_back( config["imageDir"] + "/" + *itDate + "/" + *itTime );
		}
	}

	// Load model
	cv::Ptr<cv::face::FaceRecognizer> fr;
	fr = cv::face::createEigenFaceRecognizer( 10 ); // TODO : load correct type of model
	fr->load( "/etc/pam-facial-auth/model.xml" );

	for ( std::vector<std::string>::iterator it = imagePaths.begin(); it != imagePaths.end(); ++it )
	{
		cv::Mat im      = cv::imread( *it, CV_LOAD_IMAGE_GRAYSCALE );

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
			it->c_str(), prediction, fr->getLabelInfo( prediction ).c_str(), confidence );
	}

	if ( cv::CAP_MODE_RGB != cv::CAP_MODE_RGB )
	{
		return PAM_AUTH_ERR;
	}
	printf( "rgb equals rgb... \n" );
	return PAM_SUCCESS;
}


