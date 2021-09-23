--TEST--
Vector can be cloned
--FILE--
<?php

$vec = new Vector([new stdClass(), new ArrayObject()]);
$it2 = clone $vec;
unset($vec);
foreach ($it2 as $key => $value) {
    echo "Saw entry:\n";
    var_dump($key, $value);
}

?>
--EXPECT--
Saw entry:
int(0)
object(stdClass)#2 (0) {
}
Saw entry:
int(1)
object(ArrayObject)#3 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}