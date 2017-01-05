--TEST--
Test stdin input with breakpoints
--PHPDBG--
stdin foo
<?php

echo "Hello, world!\n";
foo
b 3
r
c
r
q
--EXPECTF--
prompt> [Successful compilation of stdin input]
prompt> [Breakpoint #0 added at -:3]
prompt> [Breakpoint #0 at -:3, hits: 1]
>00003: echo "Hello, world!\n";
 00004: 
prompt> Hello, world!
[Script ended normally]
prompt> [Breakpoint #0 at -:3, hits: 1]
>00003: echo "Hello, world!\n";
 00004: 
prompt> 