--TEST--
Fundamental breakpoints functionality
--PHPDBG--
b 3
r
b 4
c

q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:3]
prompt> [Breakpoint #0 at %s:3, hits: 1]
>00003: echo $i++;
 00004: echo $i++;
 00005: echo $i++;
prompt> [Breakpoint #1 added at %s:4]
prompt> 1
[Breakpoint #1 at %s:4, hits: 1]
>00004: echo $i++;
 00005: echo $i++;
 00006: echo $i++;
prompt> 234
[Script ended normally]
prompt> 
--FILE--
<?php
$i = 1;
echo $i++;
echo $i++;
echo $i++;
echo $i++;
