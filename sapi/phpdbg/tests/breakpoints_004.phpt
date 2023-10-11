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
>00003: echo $i++;
 00004: echo $i++;
 00005: echo $i++;
prompt> 1
[Breakpoint #0 in ZEND_ECHO at %s:4, hits: 2]
>00004: echo $i++;
 00005: echo $i++;
 00006: echo $i++;
prompt> 2
[Breakpoint #0 in ZEND_ECHO at %s:5, hits: 3]
>00005: echo $i++;
 00006: echo $i++;
 00007: 
prompt> 3
[Breakpoint #0 in ZEND_ECHO at %s:6, hits: 4]
>00006: echo $i++;
 00007: 
prompt> 4
[Script ended normally]
prompt> 
--FILE--
<?php
$i = 1;
echo $i++;
echo $i++;
echo $i++;
echo $i++;
?>
