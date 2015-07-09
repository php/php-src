--TEST--
Exception inside a foreach loop with on an object with destructor
--FILE--
<?php
class bar {
	public $foo = 1;
	function __destruct() {
		throw new Exception;
	}
}

function foo() {
	foreach (new bar() as &$foo) {
		try {
			$foo = new Exception;
			return;
		} catch (Exception $e) {
			echo "Exception1\n";
		}
	}
}
try {
	foo();
} catch (Exception $e) {
	echo "Exception2\n";
}
?>
--EXPECT--
Exception2
