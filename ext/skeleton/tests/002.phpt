--TEST--
%EXTNAME%_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('%EXTNAME%')) {
	echo 'skip';
}
?>
--FILE--
<?php
$ret = %EXTNAME%_test1();

var_dump($ret);
?>
--EXPECT--
The extension %EXTNAME% is loaded and working!
NULL
