--TEST--
implode() function
--POST--
--GET--
--INI--
error_reporting=2047
log_errors=0
display_errors=0
track_errors=1
--FILE--
<?php
echo implode(array())."\n";
echo implode('nothing', array())."\n";
echo implode(array('foo', 'bar', 'baz'))."\n";
echo implode(':', array('foo', 'bar', 'baz'))."\n";
echo implode(':', array('foo', array('bar', 'baz'), 'burp'))."\n";
echo $php_errormsg."\n";
?>
--EXPECTF--


foobarbaz
foo:bar:baz
foo:Array:burp
Array to string conversion
