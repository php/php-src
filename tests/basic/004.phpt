--TEST--
Two variables in POST data
--SKIPIF--
<?php if (php_sapi_name()=='cli') echo 'skip'; ?>
--POST--
a=Hello+World&b=Hello+Again+World
--FILE--
<?php 
error_reporting(0);
echo "{$_POST['a']} {$_POST['b']}" ?>
--EXPECT--
Hello World Hello Again World
