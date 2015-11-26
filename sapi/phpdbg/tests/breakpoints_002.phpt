--TEST--
Preserve breakpoints on restart
--PHPDBG--
b breakpoints_002.php:4
r
b 3
r
y
c

q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:4]
prompt> 1
[Breakpoint #0 at %s:4, hits: 1]
>00004: echo 2;
 00005: echo 3;
 00006: echo 4;
prompt> [Breakpoint #1 added at %s:3]
prompt> Do you really want to restart execution? (type y or n): [Breakpoint #1 at %s:3, hits: 1]
>00003: echo 1;
 00004: echo 2;
 00005: echo 3;
prompt> 1
[Breakpoint #0 at %s:4, hits: 1]
>00004: echo 2;
 00005: echo 3;
 00006: echo 4;
prompt> 234
[Script ended normally]
prompt> 
--FILE--
<?php

echo 1;
echo 2;
echo 3;
echo 4;

