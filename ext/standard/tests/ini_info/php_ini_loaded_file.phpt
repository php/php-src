--TEST--
php_ini_loaded_file() function
--FILE--
<?php
    $inifile = __DIR__.DIRECTORY_SEPARATOR.'loaded.ini';
    $php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
    $code = '"var_dump(php_ini_loaded_file());"';

    // No ini file
    passthru($php.' -n -r '.$code);

    // Specified ini file
    passthru($php.' -c '.escapeshellarg($inifile).' -r '.$code);
?>
--EXPECTREGEX--
bool\(false\)
string\(\d+\) ".*[\/\\]loaded\.ini"
