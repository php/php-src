--TEST--
Trying redeclare class with class_alias()
--FILE--
<?php

class foo { }

class_alias('foo', 'foo');

?>
--EXPECTF--
Warning: Cannot redeclare class foo (previously declared in %s:%d) in %s on line %d
