--TEST--
Trying redeclare class with class_alias()
--FILE--
<?php

class foo { }

class_alias('foo', 'FOO');

?>
--EXPECTF--
Warning: Cannot declare class FOO, because the name is already in use in %s on line %d
