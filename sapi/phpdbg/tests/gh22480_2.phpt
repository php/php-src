--TEST--
GH-22480 (Use-after-free re-watching a sub-path of a recursive watchpoint)
--INI--
opcache.optimization_level=0
--PHPDBG--
b 5
r
w r $a
w $a[x]
c
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:5]
prompt> [Breakpoint #0 at %s:5, hits: 1]
>00005: $a['x'] = 2;
 00006: 
prompt> [Added watchpoint #0 for $a[]]
prompt> [$a[x] is already being watched]
prompt> [Breaking on watchpoint $a]
Old value%s
New value: Array ([x] => 2)
>00006: 
prompt> [$a has been removed, removing watchpoint recursively]
--FILE--
<?php

$a = ['x' => 1];

$a['x'] = 2;
