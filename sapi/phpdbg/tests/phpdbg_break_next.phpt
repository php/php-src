--TEST--
Test phpdbg_break_next() function
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
