--TEST--
implode() function
--POST--
--GET--
--INI--
error_reporting=2047
log_errors=0
--FILE--
<?php
echo implode(array())."\n";
echo implode('nothing', array())."\n";
echo implode(array('foo', 'bar', 'baz'))."\n";
echo implode(':', array('foo', 'bar', 'baz'))."\n";
echo implode(':', array('foo', array('bar', 'baz'), 'burp'))."\n";
?>
--EXPECTF--


foobarbaz
foo:bar:baz

Notice: Array to string conversion in %s on line 6
foo:Array:burp
