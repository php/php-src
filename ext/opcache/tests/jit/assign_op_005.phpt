--TEST--
JIT ASSIGN_OP: 005
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
$a = ["xy" => 0];
$x = "";
$a["x{$x}y"] %= 0;
?>
--EXPECTF--
Fatal error: Uncaught DivisionByZeroError: Modulo by zero in %sassign_op_005.php:4
Stack trace:
#0 {main}
  thrown in %sassign_op_005.php on line 4
