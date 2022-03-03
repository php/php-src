--TEST--
Register Alloction 007: Missing store
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test() {
    for ($i = 0; $i < 100; $i++) {
        $a = $a + $a = $a + !$a = $a;
        $aZ = $a;
        @$aZ %= $a;
    }
}
test();
?>
--EXPECTF--
Warning: Undefined variable $a in %sreg_alloc_007.php on line 4

Fatal error: Uncaught DivisionByZeroError: Modulo by zero in %sreg_alloc_007.php:6
Stack trace:
#0 %sreg_alloc_007.php(9): test()
#1 {main}
  thrown in %sreg_alloc_007.php on line 6
