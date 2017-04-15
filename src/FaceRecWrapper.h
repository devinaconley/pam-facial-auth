// =======================================================================
// pam-facial-auth
// FaceRecWrapper
//
// Created by Devin Conley
// =======================================================================

#ifndef PAM_FACIAL_AUTH_FACERECWRAPPER_H
#define PAM_FACIAL_AUTH_FACERECWRAPPER_H

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include "Utils.h"

class FaceRecWrapper
{
public:
	// Constructor
	FaceRecWrapper();
	FaceRecWrapper( const std::string & technique );

	// Exposed methods
	void Train( const std::vector<cv::Mat> & images, const std::vector<int> & labels );
	void Predict( const cv::Mat & images, int & label, double & confidence );
	void Save( const std::string & filename );
	void Load( const std::string & filename );
	void SetLabelNames( const std::vector<std::string> & names );
	std::string GetLabelName( int index );

private:
	// Helper methods
	bool SetTechnique( const std::string & t );

	// Data
	cv::Ptr<cv::face::FaceRecognizer> fr;
	std::size_t                       height;
	std::size_t                       width;
	std::string                       technique;
};


#endif //PAM_FACIAL_AUTH_FACERECWRAPPER_H
