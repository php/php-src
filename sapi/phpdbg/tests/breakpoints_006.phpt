--TEST--
Basic function breakpoints
--PHPDBG--
b foo
r
c
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at foo]
prompt> [Breakpoint #0 in foo() at %s:4, hits: 1]
>00004: 	var_dump($bar);
 00005: }
 00006: 
prompt> string(4) "test"
[Script ended normally]
prompt> 
--FILE--
<?php

function foo($bar) {
	var_dump($bar);
}

foo("test");
