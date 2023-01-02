--TEST--
Bug #72944 (Null pointer deref in zval_delref_p).
--FILE--
<?php
define('e', 'e');
("a"== e & $A = $A? 0 : 0) ?:0;
echo "OK\n";
?>
--EXPECTF--
Warning: Undefined variable $A (this will become an error in PHP 9.0) in %s on line %d
OK
