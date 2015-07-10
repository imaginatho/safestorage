#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

#include <ini_file.h>

int main ( int argc, char *argv[] )
{
	if (argc != 2) exit(-1);
	CIniFile ini(argv[1]);
	
	const char *params[6]	= { "param1", "param2", "param3", "param4", "parastr5", "p10" };
	const char *groups[3] = { "", "group-1", "group-2" };
	for(int index = -1; index < 6; ++index) {
		printf("%10s", index < 0 ? "": params[index]);
		for (int ig = 0; ig < 3; ++ig ) {
			if (index < 0) printf("|%-10s", groups[ig]);
			else printf("|%-10s", ini.internalGetParam(groups[ig], params[index]).c_str());
		}
		printf("\n");
	}
	printf("\n");
	for(int index = -1; index < 6; ++index) {
		printf("%10s", index < 0 ? "": params[index]);
		for (int ig = 0; ig < 3; ++ig ) {
			ini.setGroup(groups[ig]);
			if (index < 0) printf("|%-10s", groups[ig]);
			else printf("|%-10s", ini.getStrParam(params[index]).c_str());
		}
		printf("\n");
	}
	
	const char *patterns[] = { "/opt/safe*/readme.txt", "/opt/safe*/readme.*", "/opt/safe*/readme*kk*", "/opt/safe?/readme.*", "/opt/safe*/readme.?", NULL };
	const char *tests[] = { "/opt/safestorage/readme.txt", "/opt/safeserver/etc/readme.txt.org", "/opt/safe/readme.txt",  "/opt/safe1/readme.txt",
					        "/opt/safe/readmekkreadme", "/opt/safe/readmekkddd", "/opt/safe/readme.0", NULL };
	int ipat = 0;
	while (patterns[ipat]) {
		printf("PATTERN:%s\n", patterns[ipat]);
		int itest = 0;
		while (tests[itest]) {
			printf("match(%s, %s)=%s\n", patterns[ipat], tests[itest], CIniFile::wildcardMatch(patterns[ipat], tests[itest]) ? "TRUE":"false");
			++itest;
		}
		++ipat;
	}
	
	ini.getWildcardMatchGroup("/root/data.sfs");
	for(int index = 0; index < 6; ++index) {
		printf("getStrParam(%s)='%s'\n", params[index], ini.getStrParam(params[index]).c_str());
		printf("getIntParam(%s)=%d\n", params[index], ini.getIntParam(params[index]));
		printf("getUIntParam(%s)=%u\n", params[index], ini.getUIntParam(params[index]));
	}
}
