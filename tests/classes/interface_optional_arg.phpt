--TEST--
ZE2 An interface method allows additional default arguments
--FILE--
<?php

error_reporting(4095);

interface test {
	public function bar();
}

class foo implements test {

	public function bar($foo = NULL) {
		echo "foo\n";
	}
}

$foo = new foo;
$foo->bar();

?>
--EXPECT--
foo
