--TEST--
Test next command step over functionality
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows' && ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size')) {
    die('xfail breakpoint/watchpoint issues with JIT on Windows');
}
?>
--PHPDBG--
b 7
r
n
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:7]
prompt> [Breakpoint #0 at %s:7, hits: 1]
>00007: foo();
 00008: echo 1;
 00009: 
prompt> 0
[L8       %s ECHO                    1                                                              %s]
>00008: echo 1;
 00009: 
prompt> 
--FILE--
<?php

function foo() {
	echo 0;
}

foo();
echo 1;
