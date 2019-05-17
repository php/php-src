--TEST--
Bug #72944 (Null pointer deref in zval_delref_p).
--FILE--
<?php
define('e', 'e');
("a"== e & $A = $A? 0 : 0) ?:0;
echo "OK\n";
?>
--EXPECTF--
Notice: Undefined variable: A in %sbug72944.php on line 3
OK
