--TEST--
Basic return hints return reference check
--FILE--
<?php
interface foo {}
interface bar {
	public function &foo(array &$in) : array;
}

class qux implements bar {
	public function &foo(array &$in) : array {
		return $nothing;
	}
}

$array = [1, 2, 3];
$qux = new qux();
var_dump($qux->foo($array));
?>
--EXPECTF--
Catchable fatal error: the function qux::foo was expected to return an array and returned null in %s on line %d




