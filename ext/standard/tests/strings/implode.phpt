--TEST--
implode() function
--POST--
--GET--
--FILE--
<?php
echo implode(array())."\n";
echo implode('nothing', array())."\n";
echo implode(array('foo', 'bar', 'baz'))."\n";
echo implode(':', array('foo', 'bar', 'baz'))."\n";
echo implode(':', array('foo', array('bar', 'baz'), 'burp'))."\n";
?>
--EXPECT--


foobarbaz
foo:bar:baz
foo:Array:burp
