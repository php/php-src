--TEST--
JIT MOD: 004
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--FILE--
<?php
$a = [];
$b = "";
$a["x{$b}y"] %= 0;
?>
--EXPECTF--
Warning: Undefined array key "xy" in %smod_004.php on line 4

Fatal error: Uncaught DivisionByZeroError: Modulo by zero in %smod_004.php:4
Stack trace:
#0 {main}
  thrown in %smod_004.php on line 4