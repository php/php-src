--TEST--
ASSIGN_OP 001: Incorrect optimization of ASSIGN_OBJ may lead to memory leak
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function test() {
    $a .= $a += $y;
    $a = $a->y = '';
}
test();
?>
--EXPECTF--
Warning: Undefined variable $y in %sassign_obj_001.php on line 3

Warning: Undefined variable $a in %sassign_obj_001.php on line 3

Fatal error: Uncaught Error: Attempt to assign property "y" on string in %sassign_obj_001.php:4
Stack trace:
#0 %sassign_obj_001.php(6): test()
#1 {main}
  thrown in %sassign_obj_001.php on line 4
