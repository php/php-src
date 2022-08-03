--TEST--
JIT CMP: 008 Wrong range inference for comparison between IS_LONG and IS_FALSE/IS_TRUE
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
function test() {
    for ($i = 0; $i < 10; $i %= -4 != -4 < ($a = $a + $a)) {
    }
}
test();
?>
--EXPECTF--
Warning: Undefined variable $a in %scmp_008.php on line 3

Warning: Undefined variable $a in %scmp_008.php on line 3

Fatal error: Uncaught DivisionByZeroError: Modulo by zero in %scmp_008.php:3
Stack trace:
#0 %scmp_008.php(6): test()
#1 {main}
  thrown in %scmp_008.php on line 3
