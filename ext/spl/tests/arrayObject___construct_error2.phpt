--TEST--
SPL: ArrayObject::__construct with too many arguments.
--FILE--
<?php
echo "Too many arguments:\n";
Class C implements Iterator {
	function current() {}
	function next() {}
	function key() {}
	function valid() {}
	function rewind() {}
}

try {
  var_dump(new ArrayObject(new stdClass, 0, "C", "extra"));
} catch (TypeError $e) {
  echo $e->getMessage() . "(" . $e->getLine() .  ")\n";
}
?>
--EXPECT--
Too many arguments:
ArrayObject::__construct() expects at most 3 parameters, 4 given(12)
