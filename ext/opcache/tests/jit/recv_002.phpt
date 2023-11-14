--TEST--
JIT RECV: too few arguments
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php

function test($a)
{
}

test();

?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: Too few arguments to function test(), 0 passed in %srecv_002.php on line 7 and exactly 1 expected in %s:3
Stack trace:
#0 %s(7): test()
#1 {main}
  thrown in %s on line 3