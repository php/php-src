--TEST--
ZE2 An abstract class cannot be instantiated
--FILE--
<?php

abstract class fail {
	abstract function show();
}

class pass extends fail {
	function show() {
		echo "Call to function show()\n";
	}
}

$t2 = new pass();
$t2->show();

$t = new fail();
$t->show();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call to function show()

Fatal error: Uncaught Error: Cannot instantiate abstract class fail in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
