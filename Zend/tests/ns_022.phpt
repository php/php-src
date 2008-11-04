--TEST--
022: Name search priority (first look into import, then into current namespace and then for class)
--FILE--
<?php
namespace a\b\c;

use a\b\c as test;

require "ns_022.inc";

function foo() {
	echo __FUNCTION__,"\n";
}

test\foo();
\test::foo();
--EXPECT--
a\b\c\foo
Test::foo
