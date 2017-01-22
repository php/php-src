--TEST--
Bug #69221: Segmentation fault when using a generator in combination with an Iterator (2)
--FILE--
<?php

$gen = function() {
    yield 1;
};

$iter = new IteratorIterator($gen());
$ngen = $iter->getInnerIterator();

var_dump(iterator_to_array($ngen, false));

?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
