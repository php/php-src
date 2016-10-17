--TEST--
Test next command on function boundaries
--PHPDBG--
b 4
r
n


q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:4]
prompt> [Breakpoint #0 at %s:4, hits: 1]
>00004: 	echo 0;
 00005: }
 00006: 
prompt> 0
[L5 %s RETURN                  null                                                           %s]
>00005: }
 00006: 
 00007: foo();
prompt> [L8 %s ECHO                    1                                                              %s]
>00008: echo 1;
 00009: 
prompt> 1
[L9 %s RETURN                  1                                                              %s]
>00009: 
prompt> 
--FILE--
<?php

function foo() {
	echo 0;
}

foo();
echo 1;
