--TEST--
Bug #72944 (Null pointer deref in zval_delref_p).
--FILE--
<?php
("a"== e & $A = $A? 0 : 0) ?:0;
echo "OK\n";
?>
--EXPECTF--
Warning: Use of undefined constant e - assumed 'e' (this will throw an Error in a future version of PHP) in %sbug72944.php on line 2

Notice: Undefined variable: A in %sbug72944.php on line 2
OK
