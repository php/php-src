--TEST--
Three variables in POST data
--SKIPIF--
<?php if (php_sapi_name()=='cli') echo 'skip'; ?>
--POST--
a=Hello+World&b=Hello+Again+World&c=1
--FILE--
<?php 
error_reporting(0);
echo "{$_POST['a']} {$_POST['b']} {$_POST['c']}"?>
--EXPECT--
Hello World Hello Again World 1
