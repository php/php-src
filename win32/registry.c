#include "php.h"
#include "php_ini.h"
#include "php_registry.h"

#define MAX_NAMEBUF_LEN		512
#define MAX_VALUE_LEN		512

void UpdateIniFromRegistry(char *path)
{
	char *p, *orig_path;
	HKEY MainKey;
	char *strtok_buf = NULL;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\PHP\\Per Directory Values", 0, KEY_READ, &MainKey)!=ERROR_SUCCESS) {
		return;
	}


	orig_path = path = estrdup(path);

	/* Get rid of C:, if exists */
	p = strchr(path, ':');
	if (p) {
		*p = path[0];	/* replace the colon with the drive letter */
		path = p;		/* make path point to the drive letter */
	} else {
		if (path[0] != '\\' && path[0] != '/') {
			char tmp_buf[MAXPATHLEN], *cwd;
			char drive_letter;

			/* get current working directory and prepend it to the path */
			if (!V_GETCWD(tmp_buf, MAXPATHLEN)) {
				efree(orig_path);
				return;
			}
			cwd = strchr(tmp_buf, ':');
			if (!cwd) {
				drive_letter = 'C';
				cwd = tmp_buf;
			} else {
				drive_letter = tmp_buf[0];
				cwd++;
			}
			path = (char *) emalloc(2+strlen(cwd)+1+strlen(orig_path)+1);
			sprintf(path, "%c\\%s\\%s", drive_letter, cwd, orig_path);
			efree(orig_path);
			orig_path = path;
		}
	}


	path = p = php_strtok_r(path, "\\/", &strtok_buf);

	while (p) {
		HKEY hKey;
		char namebuf[MAX_NAMEBUF_LEN], valuebuf[MAX_VALUE_LEN];
		DWORD lType;
		DWORD namebuf_length=MAX_NAMEBUF_LEN, valuebuf_length=MAX_VALUE_LEN;
		DWORD i=0;

		if (p>path) {
			*(p-1) = '\\'; /* restore the slash */
		}
		if (RegOpenKeyEx(MainKey, path, 0, KEY_READ, &hKey)!=ERROR_SUCCESS) {
			break;
		}
		while (RegEnumValue(hKey, i++, namebuf, &namebuf_length, NULL, &lType, valuebuf, &valuebuf_length)==ERROR_SUCCESS) {
			if (lType != REG_SZ) {
				continue;
			}
			/*printf("%s -> %s\n", namebuf, valuebuf);*/
			zend_alter_ini_entry(namebuf, namebuf_length+1, valuebuf, valuebuf_length+1, PHP_INI_PERDIR, PHP_INI_STAGE_ACTIVATE);
		}

		RegCloseKey(hKey);
		p = php_strtok_r(NULL, "\\/", &strtok_buf);
	}
	RegCloseKey(MainKey);
	efree(orig_path);
}
