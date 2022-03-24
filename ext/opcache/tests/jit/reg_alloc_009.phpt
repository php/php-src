--TEST--
Register Alloction 009: Missing type store
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test() {
    for(;; $a + $y[4][] = $y < $a + $a = $b = $a = + $y[] = 0.1) {
        4 >> - $j++;
    }
}
test();
?>
--EXPECTF--
Warning: Undefined variable $j in %sreg_alloc_009.php on line 4

Fatal error: Uncaught ArithmeticError: Bit shift by negative number in %sreg_alloc_009.php:4
Stack trace:
#0 %sreg_alloc_009.php(7): test()
#1 {main}
  thrown in %sreg_alloc_009.php on line 4
