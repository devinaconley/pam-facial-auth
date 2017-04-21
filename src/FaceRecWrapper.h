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
	FaceRecWrapper( const std::string & techniqueName, const std::string & pathCascade );

	// Exposed methods
	void Train( const std::vector<cv::Mat> & images, const std::vector<int> & labels );
	void Predict( const cv::Mat & images, int & label, double & confidence );
	void Save( const std::string & path );
	void Load( const std::string & path );
	void SetLabelNames( const std::vector<std::string> & names );
	std::string GetLabelName( int index );

private:
	// Helper methods
	bool SetTechnique( const std::string & t );
	bool CropFace( const cv::Mat & image, cv::Mat & cropped );
	bool LoadCascade( const std::string & pathCascade );

	// Data
	cv::Ptr<cv::face::FaceRecognizer> fr;
	cv::CascadeClassifier             cascade;
	std::size_t                       sizeFace;
	std::string                       technique;
	std::string                       pathCascade;
};


#endif //PAM_FACIAL_AUTH_FACERECWRAPPER_H
