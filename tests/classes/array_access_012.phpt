--TEST--
ZE2 ArrayAccess cannot assign by reference
--FILE--
<?php

class ArrayAccessImpl implements ArrayAccess {
	private $data = array();

	public function offsetUnset($index) {}

	public function offsetSet($index, $value) {
		$this->data[$index] = $value;
	}

	public function offsetGet($index) {
		return $this->data[$index];
	}

	public function offsetExists($index) {
		return isset($this->data[$index]);
	}
}

$data = new ArrayAccessImpl();
$test = 'some data';
$data['element'] = NULL; // prevent notice
$data['element'] = &$test;

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--

Notice: Indirect modification of overloaded element of ArrayAccessImpl has no effect in /home/dmitry/php/php6/tests/classes/array_access_012.php on line 24

Fatal error: Objects used as arrays in post/pre increment/decrement must return values by reference in %sarray_access_012.php on line %d
