--TEST--
ASSIGN_OP 001: Incorrect optimization of ASSIGN_OP may lead to memory leak
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function test() {
    for ($i = 0; $i < 10; $i++) {
        $a = $a += $a < true;
        $a += $a;
        $a = [];
        $a['b'] += 1;
    }
}
test();
?>
--EXPECTF--
Warning: Undefined variable $a in %sassign_op_001.php on line 4

Warning: Undefined variable $a in %sassign_op_001.php on line 4

Warning: Undefined array key "b" in %sassign_op_001.php on line 7

Fatal error: Uncaught TypeError: Unsupported operand types: array + bool in %sassign_op_001.php:4
Stack trace:
#0 %sassign_op_001.php(10): test()
#1 {main}
  thrown in %sassign_op_001.php on line 4
