#include "php.h"
#include "php_ini.h"

void UpdateIniFromRegistry(char *path TSRMLS_DC)
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
			if (!VCWD_GETCWD(tmp_buf, MAXPATHLEN)) {
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
		DWORD lType;
		DWORD values = 0, max_name = 0, max_value = 0, i = 0;
		
		if (p>path) {
			*(p-1) = '\\'; /* restore the slash */
		}

		if (RegOpenKeyEx(MainKey, path, 0, KEY_READ, &hKey)!=ERROR_SUCCESS) {
			break;
		}

		if(RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &values, &max_name, &max_value, NULL, NULL) == ERROR_SUCCESS) {
			LPTSTR namebuf = (LPTSTR)emalloc(max_name + 1);
			PBYTE valuebuf = (PBYTE)emalloc(max_value);

			while (i < values) {
				DWORD namebuf_len  = max_name + 1;
				DWORD valuebuf_len = max_value;

				RegEnumValue(hKey, i, namebuf, &namebuf_len, NULL, &lType, valuebuf, &valuebuf_len);

				if ((lType == REG_SZ) || (lType == REG_EXPAND_SZ)) {
					zend_alter_ini_entry(namebuf, namebuf_len + 1, valuebuf, valuebuf_len, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
				}

				i++;
			}

			efree(namebuf);
			efree(valuebuf);
		}

		RegCloseKey(hKey);
		p = php_strtok_r(NULL, "\\/", &strtok_buf);
	}
	RegCloseKey(MainKey);
	efree(orig_path);
}
