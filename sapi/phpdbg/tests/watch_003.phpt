--TEST--
Test simple watchpoint with replace
--SKIPIF--
<?php
if (PHP_INT_SIZE == 4) {
    die("xfail There may be flaws in the implementation of watchpoints that cause failures");
}
if (PHP_OS_FAMILY === 'Windows' && ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size')) {
    die('xfail breakpoint/watchpoint issues with JIT on Windows');
}
if (getenv('SKIP_ASAN')) {
    die("skip intentionally causes segfaults");
}
?>
--PHPDBG--
b 6
r
w $a[0]
c


q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:6]
prompt> [Breakpoint #0 at %s:6, hits: 1]
>00006: $a[0] = 2;
 00007: 
 00008: $a = [0 => 3, 1 => 4];
prompt> [Added watchpoint #0 for $a[0]]
prompt> [Breaking on watchpoint $a[0]]
Old value: 1
New value: 2
>00008: $a = [0 => 3, 1 => 4];
 00009: 
prompt> [Breaking on watchpoint $a[0]]
Old value: 2
New value: 3
>00009: 
prompt> [$a[0] has been removed, removing watchpoint]
[Script ended normally]
prompt> 
--FILE--
<?php

$a = [];

$a[0] = 1;
$a[0] = 2;

$a = [0 => 3, 1 => 4];
