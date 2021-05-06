--TEST--
SPL: ArrayObject::__construct with too many arguments.
--FILE--
<?php
echo "Too many arguments:\n";
Class C implements Iterator {
    function current(): mixed {}
    function next() {}
    function key() {}
    function valid(): bool {}
    function rewind(): void {}
}

try {
  var_dump(new ArrayObject(new stdClass, 0, "C", "extra"));
} catch (TypeError $e) {
  echo $e->getMessage() . "(" . $e->getLine() .  ")\n";
}
?>
--EXPECT--
Too many arguments:
ArrayObject::__construct() expects at most 3 arguments, 4 given(12)
