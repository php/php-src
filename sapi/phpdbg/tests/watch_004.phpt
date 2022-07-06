--TEST--
Test detection of inline string manipulations on zval watch
--SKIPIF--
<?php
if (PHP_INT_SIZE == 4) {
    die("xfail There may be flaws in the implementation of watchpoints that cause failures");
}
if (getenv('SKIP_ASAN')) {
    die("skip intentionally causes segfaults");
}
?>
--INI--
opcache.optimization_level=0
--PHPDBG--
b 3
r
w $a
c


q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:3]
prompt> [Breakpoint #0 at %s:3, hits: 1]
>00003: $b = "a";
 00004: $a = $b.$b;
 00005: $a[1] = "b";
prompt> [Added watchpoint #0 for $a]
prompt> [Breaking on watchpoint $a]
Old value: 
New value: aa
>00005: $a[1] = "b";
 00006: 
prompt> [Breaking on watchpoint $a]
Old value: aa
New value: ab
>00006: 
prompt> [$a has been removed, removing watchpoint]
[Script ended normally]
prompt> 
--FILE--
<?php

$b = "a";
$a = $b.$b;
$a[1] = "b";
