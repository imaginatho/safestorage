#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

#include <ini_file.h>

CIniFile::CIniFile ( const string &filename )
{
	content = NULL;
	loadFile(filename);
	parse();
}

CIniFile::~CIniFile ( void )
{
	if (content) free(content);
	
	CIniFileGroups::iterator it = groups.begin();
	while (it != groups.end()) {
		delete it->second;
		++it;
	}
}

int CIniFile::loadFile ( const string &filename )
{
	struct stat st;	
	
	int fd = open(filename.c_str(), O_RDONLY);
	if (fd < 0) return fd;
	
	if (fstat(fd, &st) || st.st_size > MAX_INI_FILE_SIZE) {
		close(fd);
		return -1;
	}
	
	if (content) {
		free(content);
		content = NULL;
	}
	
	content = (char *)malloc(st.st_size + 1);
	memset(content, 0, sizeof(content));
	
	int bytes = read(fd, content, st.st_size);
	if (bytes > 0) content[bytes] = '\0';		
	
	close(fd);
	return (bytes >= 0 ? 0 : -1);
}

void CIniFile::parse ( void )
{
	const char *cgroup = "";
	char *eol, *key, *cvalue, *line, *pos, *current;
	int cline = 0;
	
	current = content;
	while (current && *current) {
		++cline;
		line = current;
		if ((current = strchr(line, '\n'))) {
			if (*(current - 1) == '\r') --current;
			*current = '\0';
			++current;
		}
		// clear right blanks
		eol = line + strlen(line);
		eol = line + strlen(line);
		while (eol > line && (*(eol-1) == '\t' || *(eol-1) == ' ')) --eol;
		*eol = '\0';

		// check if a comment line
		if (strchr(";/#", *line)) continue;
		
		// ignore left blanks
		while (*line == ' ' || *line == '\t') ++line;
		
		// check if empty line
		if (*line == '\0') continue;
		
		if (*line == '[') {
			// group
			pos = strchr(line, ']');
			if (!pos) exit(-2);
			*pos = '\0';
			cgroup = (line+1);
			continue;
		}
		pos = strchr(line, '=');
		
		if (!pos) exit(-3); 	
		key = line;
		
		cvalue = pos + 1;
		while (pos > line && (*(pos-1) == ' ' || *(pos-1) == '\t')) --pos;
		*pos = '\0';
	
		while (*cvalue == ' ' || *cvalue == '\t') ++cvalue;
		
		if (*cvalue == '\"' || *cvalue == '"') {
			char cquota = *cvalue;
			++cvalue;
			pos = strchr(cvalue, cquota);
			if (pos) *pos = 0;
			// buscar final de cadena y poner el '\0', siempre que no se escape la misma
		}
		addKeyValue(cgroup, key, cvalue);
	}
}

void CIniFile::addKeyValue ( const string &group, const string &key, const string &value )
{
	printf("group[%s] key[%s] value[%s]\n", group.c_str(), key.c_str(), value.c_str());
	
	if (group == "") {
		defaultGroup.values[key] = value;
		return;
	}

	CIniFileGroups::iterator it = groups.find(group);
	CIniFileGroup *pgroup;
	
	if (it == groups.end()) {
		pgroup = new CIniFileGroup;
		groups[group] = pgroup;
		gnames.push_back(group);
	}
	else pgroup = it->second;
	
	pgroup->values[key] = value;	
}

const string &CIniFile::internalGetParam ( const string &group, const string &key, const string &defaultValue )
{
	CIniFileGroups::iterator it = groups.find(group);
	CIniFileGroupValues::iterator itv;
	CIniFileGroup *pgroup;
	
	if (it != groups.end()) {
		pgroup = it->second;	
		itv = pgroup->values.find(key);
		if (itv != pgroup->values.end()) return itv->second;
	}
	
	itv = defaultGroup.values.find(key);
	if (itv != defaultGroup.values.end()) return itv->second;
	
	return defaultValue;
}

const string &CIniFile::getStrParam ( const string &key, const string &defaultValue )
{
	const string &result = internalGetParam(currentGroup, key, defaultValue);
	return result;
}


uint32_t CIniFile::getUIntParam ( const string &key, uint32_t defaultValue  )
{
	const string defvalue = "";
	const string &result = internalGetParam(currentGroup, key, defvalue);
	
	if (&result == &defvalue) return defaultValue;
	return strtoul(result.c_str(), NULL, 0);
}

int32_t CIniFile::getIntParam ( const string &key, int32_t defaultValue  )
{
	const string defvalue = "";
	const string &result = internalGetParam(currentGroup, key, defvalue);
	
	if (&result == &defvalue) return defaultValue;
	return strtol(result.c_str(), NULL, 0);
}

bool CIniFile::wildcardMatch ( const char *pattern, const char *str )
{
	while (*str) {
		switch (*pattern) 
		{
			case '?':
				if (*str == '.') return false;
				break;
			case '*':
				return !*(pattern + 1) || wildcardMatch(pattern + 1, str) || wildcardMatch(pattern, str + 1);
			default:
				if (*str != *pattern) return false;
				break;
		}
		++str;
		++pattern;
	} 
	while (*pattern == '*') ++pattern;
	return !*pattern;
}

const string &CIniFile::getWildcardMatchGroup ( const string &name, const string &defaultGroup )
{
	CIniFileGroupNames::iterator it = gnames.begin();
	
	while (it != gnames.end()) {
		if (wildcardMatch(it->c_str(), name.c_str())) {
			currentGroup = *it;
			return currentGroup;
		}
		++it;
	}
	
	currentGroup = defaultGroup;
	return defaultGroup;
}
