--TEST--
Bug #27287 (segfault with deserializing object data)
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
--FILE--
<?php

	class foo {
	}
	$foo = new foo();
	$foo->abc = 'def';

	$string = wddx_serialize_value($foo);
	$bar = wddx_deserialize($string);

	echo "OK\n";

?>
--EXPECTF--
Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
OK
