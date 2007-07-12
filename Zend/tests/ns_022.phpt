--TEST--
022: Name search priority (first look into import, then into current namespace and then for class)
--FILE--
<?php
namespace a::b::c;

import a::b::c as test;

class Test {
	static function foo() {
		echo __CLASS__,"::",__FUNCTION__,"\n";
	}
}

function foo() {
	echo __FUNCTION__,"\n";
}

test::foo();
test::test::foo();
--EXPECT--
a::b::c::foo
a::b::c::Test::foo
