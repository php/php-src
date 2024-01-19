--TEST--
Relative print commands
--INI--
opcache.enable_cli=0
--PHPDBG--
b foo
r
p
p o
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at foo]
prompt> string(4) "test"
[Breakpoint #0 in foo() at %s:15, hits: 1]
>00015: 		var_dump(strrev($baz));
 00016: 	}
 00017: 
prompt> [Stack in foo() (8 ops)]

foo:
     ; (lines=8, args=1, vars=1, tmps=2)
     ; %s:14-16
L0014 0000 CV0($baz) = RECV 1
L0015 0001 INIT_%s %d %d %s
L0015 0002 INIT_%s %d %d %s
L0015 0003 SEND_VAR CV0($baz) 1
L0015 0004 V1 = DO_ICALL
L0015 0005 SEND_VAR V1 1
L0015 0006 DO_ICALL
L0016 0007 RETURN null
prompt> L0015 0001 INIT_%s %d %d %s
prompt> 
--FILE--
<?php

namespace Foo {
	class Bar {
		function Foo($bar) {
			var_dump($bar);
		}

		function baz() { }
	}
}

namespace {
	function foo($baz) {
		var_dump(strrev($baz));
	}

	(new \Foo\Bar)->Foo("test");
	foo("test");
}
