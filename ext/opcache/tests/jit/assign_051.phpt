--TEST--
JIT ASSIGN: incorrect assignment optimization
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
function foo(int $a) {
    $arr = $a[] = (y);
    $arr = y;
    $c = $y = $arr = &y($c);
}
foo(0);
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "y" in %sassign_051.php:3
Stack trace:
#0 %sassign_051.php(7): foo(0)
#1 {main}
  thrown in %sassign_051.php on line 3
