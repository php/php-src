--TEST--
set_exception_handler() in phpdbg
--SKIPIF--
<?php
if (ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size')) {
    die('skip phpdbg is incompatible with JIT');
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
