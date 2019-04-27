--TEST--
Bug #72911 (Memleak in zend_binary_assign_op_obj_helper)
--FILE--
<?php

$a = 0;

$b = $a->b->i -= 0;

var_dump($b);

?>
--EXPECTF--
Warning: Attempt to modify property 'b' of non-object in %sbug72911.php on line %d
NULL
