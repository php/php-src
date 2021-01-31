--TEST--
ImmutableIterable can be cloned
--FILE--
<?php

$it = ImmutableIterable::fromPairs([[new stdClass(), new ArrayObject()]]);
$it2 = clone $it;
unset($it);
foreach ($it2 as $key => $value) {
    echo "Saw entry:\n";
    var_dump($key, $value);
}

?>
--EXPECT--
Saw entry:
object(stdClass)#1 (0) {
}
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
