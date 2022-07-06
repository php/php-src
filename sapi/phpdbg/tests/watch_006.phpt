--TEST--
Test multiple watch elements pointing to the same watchpoint
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
b 4
r
w $a[0]
w r $b
c






q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:4]
prompt> [Breakpoint #0 at %s:4, hits: 1]
>00004: $a[0] = 1;
 00005: $b = &$a;
 00006: $a[0] = 2;
prompt> [Added watchpoint #0 for $a[0]]
prompt> [Added recursive watchpoint #1 for $b]
prompt> [Breaking on watchpoint $a[0]]
Old value: 0
New value: 1
>00005: $b = &$a;
 00006: $a[0] = 2;
 00007: $a[1] = 3;
prompt> [Breaking on watchpoint $b]
Old value: 
New value (reference): Array ([0] => 1)
>00006: $a[0] = 2;
 00007: $a[1] = 3;
 00008: $c = [1];
prompt> [Breaking on watchpoint $a[0]]
Old value: 1
New value: 2
>00007: $a[1] = 3;
 00008: $c = [1];
 00009: $b = &$c;
prompt> [Element 1 has been added to watchpoint]
[Breaking on watchpoint $b[]]
1 elements were added to the array
>00008: $c = [1];
 00009: $b = &$c;
 00010: 
prompt> [Breaking on watchpoint $b]
Old value inaccessible or destroyed
New value (reference): Array ([0] => 2,[1] => 3)
>00009: $b = &$c;
 00010: 
prompt> [Breaking on watchpoint $b]
Old value inaccessible or destroyed
New value (reference): Array ([0] => 1)
>00010: 
prompt> [$b has been removed, removing watchpoint recursively]
[$a[0] has been removed, removing watchpoint]
[Script ended normally]
prompt> 
--FILE--
<?php

$a = [0];
$a[0] = 1;
$b = &$a;
$a[0] = 2;
$a[1] = 3;
$c = [1];
$b = &$c;
