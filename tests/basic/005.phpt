--TEST--
Three variables in POST data
--POST--
a=Hello+World&b=Hello+Again+World&c=1
--GET--
--FILE--
<?php 
error_reporting(0);
echo "$a $b $c"?>
--EXPECT--
Hello World Hello Again World 1
