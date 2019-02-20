--TEST--
Bug #72911 (Memleak in zend_binary_assign_op_obj_helper)
--FILE--
<?php

$a = 0;

$b = $a->b->i -= 0;

var_dump($b);

?>
--EXPECTF--
Notice: Trying to get property 'b' of non-object in %s on line %d

Warning: Attempt to assign property 'i' of non-object in %s on line %d
NULL
