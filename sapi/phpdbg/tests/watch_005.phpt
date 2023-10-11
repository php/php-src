--TEST--
Test proper watch comparisons when having multiple levels of indirection from a zval to its value
--SKIPIF--
<?php
if (PHP_INT_SIZE == 4) {
    die("xfail There may be flaws in the implementation of watchpoints that cause failures");
}
if (getenv('SKIP_ASAN')) {
    die("skip intentionally causes segfaults");
}
?>
--PHPDBG--
b 3
r
w r $a
c



q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:3]
prompt> [Breakpoint #0 at %s:3, hits: 1]
>00003: $b = "a";
 00004: $a = $b.$b;
 00005: $c = &$a;
prompt> [Added recursive watchpoint #0 for $a]
prompt> [Breaking on watchpoint $a]
Old value: 
New value: aa
>00005: $c = &$a;
 00006: $a[1] = "b";
 00007: 
prompt> [Breaking on watchpoint $a]
Old value inaccessible or destroyed
New value (reference): aa
>00006: $a[1] = "b";
 00007: 
 00008: exit;
prompt> [Breaking on watchpoint $a]
Old value: aa
New value: ab
>00008: exit;
 00009: 
prompt> [$a has been removed, removing watchpoint recursively]
[Script ended normally]
prompt> 
--FILE--
<?php

$b = "a";
$a = $b.$b;
$c = &$a;
$a[1] = "b";

exit;
?>
