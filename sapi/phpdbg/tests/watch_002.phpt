--TEST--
Test simple array watchpoint with replace
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) {
    die("skip intentionally causes segfaults");
}
?>
--PHPDBG--
b 6
r
w a $a
c

q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:6]
prompt> [Breakpoint #0 at %s:6, hits: 1]
>00006: $a[0] = 2;
 00007: 
 00008: $a = [0 => 3, 1 => 4];
prompt> [Added watchpoint #0 for $a[]]
prompt> [Breaking on watchpoint $a[]]
1 elements were added to the array
>00009: 
prompt> [$a[] has been removed, removing watchpoint]
[Script ended normally]
prompt> 
--FILE--
<?php

$a = [];

$a[0] = 1;
$a[0] = 2;

$a = [0 => 3, 1 => 4];
?>
