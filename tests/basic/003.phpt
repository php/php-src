--TEST--
GET and POST Method combined
--SKIPIF--
<?php if (php_sapi_name()=='cli') print 'skip';?>
--POST--
a=Hello+World
--GET--
b=Hello+Again+World&c=Hi+Mom
--FILE--
<?php 
error_reporting(0);
echo "{$_POST['a']} {$_GET['b']} {$_GET['c']}"?>
--EXPECT--
Hello World Hello Again World Hi Mom
