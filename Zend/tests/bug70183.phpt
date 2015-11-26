--TEST--
Bug #70183 (null pointer deref (segfault) in zend_eval_const_expr)
--FILE--
<?php
[[][]]
?>
--EXPECTF--
Fatal error: Cannot use [] for reading in %sbug70183.php on line %d
