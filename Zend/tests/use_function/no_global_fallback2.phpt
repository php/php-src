--TEST--
non-existent imported functions should not be looked up in the global table
--FILE--
<?php

namespace {
	function test() {
		echo "NO!";
	}
}
namespace foo {
	use function bar\test;
	test();
}

?>
--EXPECTF--
Fatal error: Call to undefined function bar\test() in %s on line %d
