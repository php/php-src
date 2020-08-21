--TEST--
Test next command step over functionality
--PHPDBG--
b 7
r
n
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:7]
prompt> [Breakpoint #0 at %s:7, hits: 1]
>00007: foo();
 00008: echo 1;
 00009: 
prompt> 0
[L8       %s ECHO                    1                                                              %s]
>00008: echo 1;
 00009: 
prompt> 
--FILE--
<?php

function foo() {
	echo 0;
}

foo();
echo 1;
