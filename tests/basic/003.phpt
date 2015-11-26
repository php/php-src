--TEST--
GET and POST Method combined
--POST--
a=Hello+World
--GET--
b=Hello+Again+World&c=Hi+Mom
--FILE--
<?php 
error_reporting(0);
echo "post-a=({$_POST['a']}) get-b=({$_GET['b']}) get-c=({$_GET['c']})"?>
--EXPECT--
post-a=(Hello World) get-b=(Hello Again World) get-c=(Hi Mom)
