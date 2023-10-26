--TEST--
JIT IDENTICAL: 004 undefined error
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
function foo() {
    $y === $y;
}
foo();
?>
DONE
--EXPECTF--
Warning: Undefined variable $y in %sidentical_004.php on line 3

Warning: Undefined variable $y in %sidentical_004.php on line 3
DONE
