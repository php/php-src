--TEST--
Two variables in POST data
--POST--
a=Hello+World&b=Hello+Again+World
--GET--
--FILE--
<?php 
error_reporting(0);
echo "$a $b"?>
--EXPECT--
Hello World Hello Again World
