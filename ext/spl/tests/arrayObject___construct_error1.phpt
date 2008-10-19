--TEST--
SPL: ArrayObject::__construct with bad iterator.
--FILE--
<?php
echo "Bad iterator type:\n";
$a = new stdClass;
$a->p = 1;
try {
  var_dump(new ArrayObject($a, 0, "Exception"));
} catch (InvalidArgumentException $e) {
  echo $e->getMessage() . "(" . $e->getLine() .  ")\n";
}

echo "Non-existent class:\n";
try {
  var_dump(new ArrayObject(new stdClass, 0, "nonExistentClassName"));
} catch (InvalidArgumentException $e) {
  echo $e->getMessage() . "(" . $e->getLine() .  ")\n";
}
?>
--EXPECTF--
Bad iterator type:
ArrayObject::__construct() expects parameter 3 to be a class name derived from Iterator, 'Exception' given(6)
Non-existent class:
ArrayObject::__construct() expects parameter 3 to be a class name derived from Iterator, 'nonExistentClassName' given(13)
