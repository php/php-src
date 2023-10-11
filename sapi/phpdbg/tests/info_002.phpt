--TEST--
info constants test
--PHPDBG--
b 10
r
i d
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:10]
prompt> [Breakpoint #0 at %s:10, hits: 1]
>00010: print B;
 00011: 
prompt> [User-defined constants (2)]
Address            Refs    Type      Constant
%s 1       int       A
int (10)
%s 1       int       B
int (100)
prompt>
--FILE--
<?php

const A = 10;
const B = C::D * A;

class C {
	const D = 10;
}

print B;
?>
