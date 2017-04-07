// =======================================================================
// pam-facial-auth
// Util
//
// Created by Devin Conley
// =======================================================================

#ifndef PAM_FACIAL_AUTH_UTIL_H
#define PAM_FACIAL_AUTH_UTIL_H

#include <dirent.h>
#include <fstream>

class Util
{
public:
	static void GetConfig( const std::string & pathConfig, std::map<std::string, std::string> & config )
	{
		// try to pull config
		std::ifstream inFile( pathConfig.c_str() );
		if ( !inFile )
		{
			printf( "Cannot open config file: %s\n", pathConfig.c_str() );
			return;
		}

		std::stringstream ssFile;

		ssFile << inFile.rdbuf();
		inFile.close();

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
	}

	static void WalkDirectory( const std::string & pathDir,
		std::vector<std::string> & files, std::vector<std::string> & subs )
	{
		DIR           * dirMain;
		DIR           * dirSub;
		struct dirent * curr;

		dirMain = opendir( pathDir.c_str() );
		if ( dirMain )
		{
			// iterate through elements
			curr = readdir( dirMain );
			while ( curr )
			{
				if ( std::strcmp( curr->d_name, "." ) == 0 ||
					 std::strcmp( curr->d_name, ".." ) == 0 )
				{
					curr = readdir( dirMain );
					continue;
				}
				// check if dir or file
				dirSub = opendir( ( pathDir + "/" + curr->d_name ).c_str() );
				if ( dirSub )
				{
					subs.push_back( curr->d_name );
				}
				else
				{
					files.push_back( curr->d_name );
				}
				curr = readdir( dirMain );
			}
		}

	}
};


#endif //PAM_FACIAL_AUTH_UTIL_H
