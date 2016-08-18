--TEST--
Test namespaced and non-lowercase breakpoint names
--PHPDBG--
b foo\bar::foo
b \Foo\Bar::Foo
r
c
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at foo\bar::foo]
prompt> [Breakpoint exists at Foo\Bar::Foo]
prompt> [Breakpoint #0 in foo\bar::foo() at %s:6, hits: 1]
>00006: 			var_dump($bar);
 00007: 		}
 00008: 	}
prompt> string(4) "test"
[Script ended normally]
prompt> 
--FILE--
<?php

namespace Foo {
	class Bar {
		function Foo($bar) {
			var_dump($bar);
		}
	}
}

namespace {
	(new \Foo\Bar)->Foo("test");
}

