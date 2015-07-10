#ifndef __INI_FILE_H__
#define __INI_FILE_H__

#include <string>
#include <list>
#include <map>

using namespace std;

#ifndef MAX_INI_FILE_SIZE
#define MAX_INI_FILE_SIZE (512 * 1024)
#endif 

typedef map<string, string> CIniFileGroupValues;

class CIniFileGroup
{
	public:
		CIniFileGroupValues values;
};

typedef map<string, CIniFileGroup *> CIniFileGroups;
typedef list<string> CIniFileGroupNames;

class CIniFile
{
	protected:
		char *content;
		string currentGroup;
		CIniFileGroups groups;
		CIniFileGroup defaultGroup;
		CIniFileGroupNames gnames;		// to get same order that ini file
		
		int loadFile ( const string &filename );
		void parse ( void );
		void addKeyValue ( const string &group, const string &key, const string &value );
	public:
		CIniFile ( const string &filename );
		~CIniFile ( void );
		const string &internalGetParam ( const string &group, const string &key, const string &defaultValue = "" );
		const string &getStrParam ( const string &key, const string &defaultValue = "" );
		void setGroup ( const string &group ) { currentGroup = group; };
		uint32_t getUIntParam ( const string &key, uint32_t defaultValue = 0 );
		int32_t getIntParam ( const string &key, int32_t defaultValue = 0  );
		const string &getWildcardMatchGroup ( const string &name, const string &defaultvalue = "" );
		static bool wildcardMatch ( const char *pattern, const char *str );
};

#endif