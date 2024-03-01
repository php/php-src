--TEST--
Basic print functionality
--INI--
opcache.enable_cli=0
--PHPDBG--
p foo
p class \Foo\bar
p
p e
q
--EXPECTF--
[Successful compilation of %s]
prompt> [User Function foo (8 ops)]

foo:
     ; (lines=8, args=1, vars=1, tmps=2)
     ; %s:14-16
L0014 0000 CV0($baz) = RECV 1
L0015 0001 INIT_FCALL %d %d %s
L0015 0002 INIT_FCALL %d %d %s
L0015 0003 SEND_VAR CV0($baz) 1
L0015 0004 V1 = DO_ICALL
L0015 0005 SEND_VAR V1 1
L0015 0006 DO_ICALL
L0016 0007 RETURN null
prompt> [User Class: Foo\Bar (2 methods)]

Foo\Bar::Foo:
     ; (lines=5, args=1, vars=1, tmps=1)
     ; %s:5-7
L0005 0000 CV0($bar) = RECV 1
L0006 0001 INIT_NS_FCALL_BY_NAME 1 string("Foo\\var_dump")
L0006 0002 SEND_VAR_EX CV0($bar) 1
L0006 0003 DO_FCALL
L0007 0004 RETURN null

Foo\Bar::baz:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; %s:9-9
L0009 0000 RETURN null
prompt> [Not Executing!]
prompt> [Context %s (9 ops)]

$_main:
     ; (lines=9, args=0, vars=0, tmps=4)
     ; %s:1-21
L0018 0000 V0 = NEW 0 string("Foo\\Bar")
L0018 0001 DO_FCALL
L0018 0002 INIT_METHOD_CALL 1 V0 string("Foo")
L0018 0003 SEND_VAL_EX string("test \"quotes\"") 1
L0018 0004 DO_FCALL
L0019 0005 INIT_FCALL %d %d %s
L0019 0006 SEND_VAL string("test") 1
L0019 0007 DO_FCALL
L0021 0008 RETURN int(1)
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

	(new \Foo\Bar)->Foo('test "quotes"');
	foo("test");
}
