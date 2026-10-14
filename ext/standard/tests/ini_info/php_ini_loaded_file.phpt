--TEST--
php_ini_loaded_file() function
--FILE--
<?php
    $inifile = escapeshellarg(__DIR__.DIRECTORY_SEPARATOR.'loaded.ini');
    $php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
    $args = getenv('TEST_PHP_EXTRA_ARGS');
    $code = '"var_dump(php_ini_loaded_file());"';

    // No ini file
    passthru("$php $args -n -r $code");

    // Specified ini file
    passthru("$php $args -c $inifile -r $code");
?>
--EXPECTREGEX--
bool\(false\)
string\(\d+\) ".*[\/\\]loaded\.ini"
