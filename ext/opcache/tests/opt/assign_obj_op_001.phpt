--TEST--
ASSIGN_OBJ_OP 001: Incorrect optimization of ASSIGN_OBJ_OP may lead to memory leak
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function s() {
    $a .= $a += $y;
    $a = $a->y *= '';
}
s();
?>
--EXPECTF--
Warning: Undefined variable $y in %sassign_obj_op_001.php on line 3

Warning: Undefined variable $a in %sassign_obj_op_001.php on line 3

Fatal error: Uncaught Error: Attempt to assign property "y" on string in %sassign_obj_op_001.php:4
Stack trace:
#0 %sassign_obj_op_001.php(6): s()
#1 {main}
  thrown in %sassign_obj_op_001.php on line 4
