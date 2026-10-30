--TEST--
JIT ASSIGN_DIM: 016
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function foo() {
    $y[~$y] = 42;
}
@foo();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot perform bitwise not on null in %sassign_dim_016.php:3
Stack trace:
#0 %sassign_dim_016.php(5): foo()
#1 {main}
  thrown in %sassign_dim_016.php on line 3
