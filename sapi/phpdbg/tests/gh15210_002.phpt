--TEST--
GH-15210 use after free after continue
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) {
    die("skip intentionally causes segfaults");
}
?>
--PHPDBG--
b 4
r
w $a[0]
c
c
q
--FILE--
<?php
header_register_callback(function() { echo "sent";});
$a = [0];
$a[0] = 1;
?>
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:%d]
prompt> [Breakpoint #0 at %s:%d, hits: 1]
>00004: $a[0] = 1;
 00005: ?>
 00006: 
prompt> [Added watchpoint #0 for $a[0]]
prompt> [Breaking on watchpoint $a[0]]
Old value: [Breaking on watchpoint $a[0]]
Old value: 0
New value: 1
>00002: header_register_callback(function() { echo "sent";});
 00003: $a = [0];
 00004: $a[0] = 1;
prompt> sent0
New value: 1

[$a[0] has been removed, removing watchpoint]
[Script ended normally]
prompt>
