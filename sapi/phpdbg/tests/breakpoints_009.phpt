--TEST--
Basic conditional breakpoint functionality
--PHPDBG--
b @ 3 if $i > 1
r
b @ 4 if $i > 2
info break
c
b @ 5 if $i > 10
c
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Conditional breakpoint #0 added %s]
prompt> [Conditional breakpoint #0: at %s:3#3 %s at %s:3, hits: 1]
>00003: echo $i++;
 00004: echo $i++;
 00005: echo $i++;
prompt> [Conditional breakpoint #1 added %s]
prompt> ------------------------------------------------
Conditional Breakpoints:
#0%wat %s:3 if $i > 1
#1%wat %s:4 if $i > 2
prompt> 1
[Conditional breakpoint #1: at %s:4#4 %s at %s:4, hits: 1]
>00004: echo $i++;
 00005: echo $i++;
 00006: echo $i++;
prompt> [Conditional breakpoint #2 added %s]
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
