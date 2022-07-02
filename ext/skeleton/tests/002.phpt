--TEST--
test1() Basic test
--EXTENSIONS--
%EXTNAME%
--FILE--
<?php
$ret = test1();

var_dump($ret);
?>
--EXPECT--
The extension %EXTNAME% is loaded and working!
NULL
