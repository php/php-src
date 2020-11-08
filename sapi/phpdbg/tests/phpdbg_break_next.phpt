--TEST--
Test phpdbg_break_next() function
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
prompt> A
[Breakpoint #0 added at %s]
[Breakpoint #0 in %s at %s:5, hits: 1]
>00005: echo 'B';
 00006: 
prompt> B
[Script ended normally]
prompt> 
--FILE--
<?php

echo 'A';
phpdbg_break_next();
echo 'B';
