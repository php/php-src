#include "php.h"
#include "php_ini.h"
#include "php_registry.h"

void UpdateIniFromRegistry(char *path)
{
	char *p, *orig_path;
	HKEY MainKey;


	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\PHP\\Per Directory Values", 0, KEY_READ, &MainKey)!=ERROR_SUCCESS) {
		return;
	}


	orig_path = path = estrdup(path);

	/* Get rid of C:, if exists */
	p = strchr(path, ':');
	if (p) {
		path = p+1;
	} else {
		if (path[0] != '\\' && path[0] != '/') {
			char tmp_buf[MAXPATHLEN], *cwd;

			/* get current working directory and prepend it to the path */
			if (!getcwd(tmp_buf, MAXPATHLEN)) {
				efree(orig_path);
				return;
			}
			cwd = strchr(tmp_buf, ':');
			if (!cwd) {
				cwd = tmp_buf;
			} else {
				cwd++;
			}
			path = (char *) emalloc(strlen(cwd)+1+strlen(orig_path)+1);
			sprintf(path, "%s\\%s", cwd, orig_path);
			efree(orig_path);
			orig_path = path;
		}
	}


	path++;	/* step over the first / */
	path = p = strtok(path, "\\/");

	while (p) {
		HKEY hKey;
		char namebuf[256], valuebuf[256];
		DWORD lType;
		DWORD namebuf_length=256, valuebuf_length=256;
		DWORD i=0;

		if (p>path) {
			*(p-1) = '\\';
		}
		if (RegOpenKeyEx(MainKey, path, 0, KEY_READ, &hKey)!=ERROR_SUCCESS) {
			break;
		}
		while (RegEnumValue(hKey, i++, namebuf, &namebuf_length, NULL, &lType, valuebuf, &valuebuf_length)==ERROR_SUCCESS) {
			if (lType != REG_SZ) {
				continue;
			}
			printf("%s -> %s\n", namebuf, valuebuf);
			php_alter_ini_entry(namebuf, namebuf_length+1, valuebuf, valuebuf_length+1, PHP_INI_PERDIR);
		}

		RegCloseKey(hKey);
		p = strtok(NULL, "\\/");
	}
	RegCloseKey(MainKey);
	efree(orig_path);
}
