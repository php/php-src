--TEST--
SPL: ArrayObject::__construct with bad iterator.
--FILE--
<?php
echo "Bad iterator type:\n";
$a = new stdClass;
$a->p = 1;
try {
  var_dump(new ArrayObject($a, 0, "Exception"));
} catch (TypeError $e) {
  echo $e->getMessage() . "(" . $e->getLine() .  ")\n";
}

echo "Non-existent class:\n";
try {
  var_dump(new ArrayObject(new stdClass, 0, "nonExistentClassName"));
} catch (TypeError $e) {
  echo $e->getMessage() . "(" . $e->getLine() .  ")\n";
}
?>
--EXPECT--
Bad iterator type:
ArrayObject::__construct(): Argument #3 ($iteratorClass) must be a class name derived from ArrayIterator, Exception given(6)
Non-existent class:
ArrayObject::__construct(): Argument #3 ($iteratorClass) must be a class name derived from ArrayIterator, nonExistentClassName given(13)
