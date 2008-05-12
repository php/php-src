--TEST--
Trying redeclare class with class_alias()
--FILE--
<?php

class foo { }

class_alias('foo', 'FOO');

?>
--EXPECTF--
Warning: Cannot redeclare class FOO in %s on line %d
