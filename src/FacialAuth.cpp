#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <opencv2/opencv.hpp>

// forward declarations
std::map<std::string, std::string> GetConfig( const std::string & pathFile );

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
	std::map<std::string, std::string> config = GetConfig( "/etc/pam-facial-auth/config" );

	// Find recent images of interest
	printf( "%s, %s\n", config["imageDir"].c_str(), config["imageWindow"].c_str() );

	if ( cv::CAP_MODE_RGB != cv::CAP_MODE_RGB )
	{
		return PAM_AUTH_ERR;
	}
	printf( "rgb equals rgb... \n" );
	return PAM_SUCCESS;
}

std::map<std::string, std::string> GetConfig( const std::string & pathConfig )
{
	std::map<std::string, std::string> config;

	// defaults
	config["imageDir"]    = "/var/lib/motioneye/Camera1";
	config["imageWindow"] = "5";

	// try to pull config
	std::istringstream ssFile( pathConfig );

	std::string line;
	while ( std::getline( ssFile, line ) )
	{
		std::istringstream ssLine( line );
		std::string        key;
		if ( std::getline( ssLine, key, '=' ) )
		{
			std::string value;
			if ( std::getline( ssLine, value ) )
			{
				config[key] = value;
			}
		}
	}
	return config;
}
