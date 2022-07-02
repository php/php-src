--TEST--
SPL: iterator_to_array() without keys
--FILE--
<?php

$it = new AppendIterator();
$it->append(new ArrayIterator(array(1,2)));
$it->append(new ArrayIterator(array(2,3)));

var_dump(iterator_to_array($it));
var_dump(iterator_to_array($it, false));
var_dump(iterator_to_array($it, true));

?>
--EXPECT--
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
