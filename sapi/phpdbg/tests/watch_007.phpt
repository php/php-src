--TEST--
Test malformed watchpoint name
--INI--
opcache.optimization_level=0
--PHPDBG--
b test
r
w $>
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at test]
prompt> [Breakpoint #0 in test() at %s:%d, hits: 1]
>00004: }
 00005: test();
 00006: $a = 2;
prompt> [Malformed input]
prompt>
--FILE--
<?php
$a = 1;
function test() {
}
test();
$a = 2;
