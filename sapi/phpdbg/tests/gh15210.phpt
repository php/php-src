--TEST--
GH-15210 use after free after continue
--CREDITS--
YuanchengJiang
--PHPDBG--
b 4
r
w $a[0]
w r $b
c
q
--FILE--
<?php
header_register_callback(function() { echo "sent";});
$a = [0];
$a[0] = 1;
$b = &$a;
$a[0] = 2;
$a[1] = 3;
$c = [1];
$b = &$c;
?>
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:%d]
prompt> [Breakpoint #0 at %s:%d, hits: 1]
>00004: $a[0] = 1;
 00005: $b = &$a;
 00006: $a[0] = 2;
prompt> [Added watchpoint #0 for $a[0]]
prompt> [Added recursive watchpoint #1 for $b]
prompt> [Breaking on watchpoint $a[0]]
Old value: [Breaking on watchpoint $a[0]]
Old value: 0
New value: 1
>00002: header_register_callback(function() { echo "sent";});
 00003: $a = [0];
 00004: $a[0] = 1;
prompt> [$a[0] has been removed, removing watchpoint]
[$b has been removed, removing watchpoint recursively]
