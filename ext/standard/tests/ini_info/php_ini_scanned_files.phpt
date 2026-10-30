--TEST--
php_ini_scanned_files() function
--FILE--
<?php
    $inifile = __DIR__.DIRECTORY_SEPARATOR.'loaded.ini';
    $php = sprintf('%s -c %s', getenv('TEST_PHP_EXECUTABLE_ESCAPED'), escapeshellarg($inifile));
    $scandir = __DIR__.DIRECTORY_SEPARATOR.'scandir';

    // Empty env value disables any config option
    putenv('PHP_INI_SCAN_DIR=');
    passthru($php.' -r "var_dump(php_ini_scanned_files());"');

    // Env value without path separator overrides any config option
    putenv('PHP_INI_SCAN_DIR='.$scandir);
    passthru($php.' -r "var_dump(php_ini_scanned_files());"');

    // Scanned ini values override previously loaded values
    passthru($php.' -r "var_dump(ini_get(\'date.timezone\'));"');
?>
--EXPECTREGEX--
bool\(false\)
string\(\d+\) ".*[\/\\]0\.ini
"
string\(\d+\) "Antarctica\/McMurdo"
