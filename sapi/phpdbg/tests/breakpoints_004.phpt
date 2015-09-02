--TEST--
Test opcode breakpoints
--PHPDBG--
b ZEND_ECHO
r
c



q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at ZEND_ECHO]
prompt> [Breakpoint #0 in ZEND_ECHO at %s:3, hits: 1]
>00003: echo 1;
 00004: echo 2;
 00005: echo 3;
prompt> 1
[Breakpoint #0 in ZEND_ECHO at %s:4, hits: 2]
>00004: echo 2;
 00005: echo 3;
 00006: echo 4;
prompt> 2
[Breakpoint #0 in ZEND_ECHO at %s:5, hits: 3]
>00005: echo 3;
 00006: echo 4;
 00007: 
prompt> 3
[Breakpoint #0 in ZEND_ECHO at %s:6, hits: 4]
>00006: echo 4;
 00007: 
prompt> 4
[Script ended normally]
prompt> 
--FILE--
<?php

echo 1;
echo 2;
echo 3;
echo 4;
