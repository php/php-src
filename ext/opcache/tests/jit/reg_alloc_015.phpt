--TEST--
Register Alloction 015: Missing store
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function foo() {
    for ($i=0, $y = $a + $y = $a = $y %= !$y; $i < 5; $a = $y < $y = $a, $i++) {
        4 >> -$y;
    }
}
foo()
?>
DONE
--EXPECTF--
Warning: Undefined variable $y in %sreg_alloc_015.php on line 3

Warning: Undefined variable $y in %sreg_alloc_015.php on line 3
DONE
