--TEST--
Register Alloction 013: Division by zero
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function foo() {
    $j++;
    $j++ % $j -= $a % $a = $j;
}
foo();
?>
DONE
--EXPECTF--
Warning: Undefined variable $j in %sreg_alloc_013.php on line 3
DONE