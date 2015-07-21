--TEST--
active_class_entry must be always correct (__METHOD__ should not depend on declaring function ce)
--FILE--
<?php

namespace Baz;

class Foo {
	public static function bar() {
		function foo() {
			var_dump(__FUNCTION__);
			var_dump(__METHOD__);
		}

		foo();
		
		var_dump(__FUNCTION__);
		var_dump(__METHOD__);
	}
}

Foo::bar();

?>
--EXPECT--
string(7) "Baz\foo"
string(7) "Baz\foo"
string(3) "bar"
string(12) "Baz\Foo::bar"
