--TEST--
Test simple recursive watchpoint
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) {
    die("skip intentionally causes segfaults");
}
?>
--INI--
opcache.optimization_level=0
--PHPDBG--
b 3
r
w r $b
c



q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:3]
prompt> [Breakpoint #0 at %s:3, hits: 1]
>00003: $a = 1;
 00004: $b = [$a];
 00005: 
prompt> [Added recursive watchpoint #0 for $b]
prompt> [Breaking on watchpoint $b]
Old value: 
New value: Array ([0] => 1)
>00006: unset($b);
 00007: $b = 2;
 00008: 
prompt> [Breaking on watchpoint $b]
Old value inaccessible or destroyed
New value: 
>00007: $b = 2;
 00008: 
prompt> [Breaking on watchpoint $b]
Old value: 
New value: 2
>00008: 
prompt> [$b has been removed, removing watchpoint recursively]
[Script ended normally]
prompt> 
--FILE--
<?php

$a = 1;
$b = [$a];

unset($b);
$b = 2;
