--TEST--
JIT MUL: 009 memory leak
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--FILE--
<?php
$x[""][] = 1;
$x[~"$y"] *= 1;
?>
--EXPECTF--
Warning: Undefined variable $y in %smul_009.php on line 3

Fatal error: Uncaught TypeError: Unsupported operand types: array * int in %smul_009.php:3
Stack trace:
#0 {main}
  thrown in %smul_009.php on line 3