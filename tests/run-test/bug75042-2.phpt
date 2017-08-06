--TEST--
phpt EXTENSIONS directive with static module
--SKIPIF--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
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
