--TEST--
php_strip_whitespace() and output buffer
--SKIPIF--
<?php if( substr(PHP_OS, 0, 3) != "WIN") die('skip Windows only test');?>
--FILE--
<?php
$file = str_repeat("A", PHP_MAXPATHLEN - strlen(__DIR__ . DIRECTORY_SEPARATOR . __FILE__));

var_dump(php_strip_whitespace($file));
var_dump(ob_get_contents());

?>
--EXPECTF--
Warning: php_strip_whitespace(%s): Failed to open stream: No such file or directory in %s on line %d
string(0) ""
bool(false)
