--TEST--
implode() function
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

Warning: Array to string conversion in %s on line %d
foo:Array:burp
