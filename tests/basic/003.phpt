--TEST--
GET and POST Method combined
--POST--
a=Hello+World
--GET--
b=Hello+Again+World&c=Hi+Mom
--FILE--
<?php 
error_reporting(0);
echo "$a $b $c"?>
--EXPECT--
Hello World Hello Again World Hi Mom
