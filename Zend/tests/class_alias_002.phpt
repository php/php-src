--TEST--
Trying redeclare class with class_alias()
--FILE--
<?php

class foo { }

class_alias('foo', 'FOO');

?>
--EXPECTF--
Warning: class FOO cannot be declared, because the name is already in use in %s on line %d
