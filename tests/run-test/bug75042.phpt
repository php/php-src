--TEST--
phpt EXTENSIONS directive with shared module
--SKIPIF--
<?php
if(empty($_ENV['TEST_PHP_EXECUTABLE'])) {
	die('skip TEST_PHP_EXECUTABLE not set');
}
$php = $_ENV['TEST_PHP_EXECUTABLE'];
if (false !== stripos(`$php -n -m`, 'openssl')) {
	die('skip openssl is built static');
}
$ext_module = ini_get('extension_dir') . DIRECTORY_SEPARATOR . (substr(PHP_OS, 0, 3) === "WIN" ? "php_openssl." : "openssl.") . PHP_SHLIB_SUFFIX;
if( !file_exists($ext_module) ) die('skip openssl shared extension not found');
--EXTENSIONS--
openssl
--FILE--
<?php
var_dump(extension_loaded('openssl'));
--EXPECT--
bool(true)