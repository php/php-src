--TEST--
phpt EXTENSIONS directive with static module
--SKIPIF--
<?php
if(empty($_ENV['TEST_PHP_EXECUTABLE'])) {
	die('skip TEST_PHP_EXECUTABLE not set');
}
$php = $_ENV['TEST_PHP_EXECUTABLE'];
if (false === stripos(`$php -n -m`, 'spl')) {
	die('skip spl is NOT built static');
}
--EXTENSIONS--
SPL
--FILE--
<?php
var_dump(extension_loaded('spl'));
--EXPECT--
bool(true)