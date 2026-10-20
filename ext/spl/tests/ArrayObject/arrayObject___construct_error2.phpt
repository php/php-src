--TEST--
SPL: ArrayObject::__construct with too many arguments.
--FILE--
<?php
echo "Too many arguments:\n";
Class C implements Iterator {
    function current(): mixed {}
    function next(): void {}
    function key(): mixed {}
    function valid(): bool {}
    function rewind(): void {}
}

try {
  var_dump(new ArrayObject(new stdClass, 0, "C", "extra"));
} catch (TypeError $e) {
  echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Too many arguments:
ArgumentCountError: ArrayObject::__construct() expects at most 3 arguments, 4 given
