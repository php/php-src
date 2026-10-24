--TEST--
php_ini_loaded_file() function
--FILE--
<?php
    $inifile = escapeshellarg(__DIR__.DIRECTORY_SEPARATOR.'loaded.ini');
    $php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
    $args = getenv('TEST_PHP_EXTRA_ARGS');
    $code = '"var_dump(php_ini_loaded_file());"';

    // No ini file. Do not pass TEST_PHP_EXTRA_ARGS here: it can contain a
    // -c option (e.g. when run-tests.php is invoked with -c, as "make test"
    // does) which takes precedence over -n.
    passthru("$php -n -r $code");

    // Specified ini file
    passthru("$php $args -c $inifile -r $code");
?>
--EXPECTREGEX--
bool\(false\)
string\(\d+\) ".*[\/\\]loaded\.ini"
