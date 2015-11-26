--TEST--
Test basic info functionality
--PHPDBG--
i classes
i funcs
b foo
r
i v
i g
i b
i d
i F
i e
i l
c
i v
q
--EXPECTF--
[Successful compilation of %s]
prompt> [User Classes (1)]
User Class Foo\Bar (2)
|---- in %s on line 4
prompt> [User Functions (1)]
|-------- foo in %s on line 14
prompt> [Breakpoint #0 added at foo]
prompt> string(4) "test"
[Breakpoint #0 in foo() at %s:15, hits: 1]
>00015: 		var_dump(strrev($baz));
 00016: 	}
 00017: 
prompt> [Variables in foo() (1)]
Address            Refs    Type      Variable
%s 1       string    $baz
string (4) "test"
prompt> [Superglobal variables (8)]
Address            Refs    Type      Variable
%s 2       array     $_GET
%s 2       array     $_POST
%s 2       array     $_COOKIE
%s 2       array     $_FILES
%s 1       array     &$GLOBALS
%s 2       array     $_SERVER
%s 2       array     $_ENV
%s 1       array     $_REQUEST
prompt> ------------------------------------------------
Function Breakpoints:
#0		foo
prompt> [User-defined constants (0)]
prompt> [Included files: 0]
prompt> [No error found!]
prompt> [Literal Constants in foo() (2)]
|-------- C0 -------> [var_dump]
|-------- C1 -------> [strrev]
prompt> string(4) "tset"
[Script ended normally]
prompt> [No active op array!]
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
