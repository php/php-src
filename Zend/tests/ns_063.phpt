--TEST--
063: Support for old-style constructors in namesapces
--FILE--
<?php
namespace Foo;
class Bar {
	function Bar() {
		echo "ok\n";
	}
}
new Bar();
--EXPECT--
ok
