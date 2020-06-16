--TEST--
set_exception_handler() in phpdbg
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows' && ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size')) {
    die('xfail breakpoint/watchpoint issues with JIT on Windows');
}
?>
--PHPDBG--
r
c
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Uncaught Exception in %s on line 4: test]
>00004: throw new Exception("test");
 00005: 
prompt> EX
[Script ended normally]
prompt> 
--FILE--
<?php

set_exception_handler(function () { print "EX\n"; });
throw new Exception("test");
