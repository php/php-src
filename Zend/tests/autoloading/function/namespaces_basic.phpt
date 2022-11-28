--TEST--
Check functions found on fallback to root.
--FILE--
<?php
namespace {
	function foo()
	{
		echo "I am just foo.\n";
	}
}

namespace bar {
	foo();
}
?>
--EXPECT--
I am just foo.
