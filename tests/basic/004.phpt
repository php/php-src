--TEST--
Two variables in POST data
--POST--
a=Hello+World&b=Hello+Again+World
--GET--
--FILE--
<?php 
error_reporting(0);
echo "{$_POST['a']} {$_POST['b']}" ?>
--EXPECT--
Hello World Hello Again World
