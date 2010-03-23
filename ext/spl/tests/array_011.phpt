--TEST--
SPL: ArrayIterator, LimitIterator and string keys
--FILE--
<?php

$a = array('zero' => 0, 'one' => 1, 'two' => 2, 'three' => 3, 'four' => 4, 'five' => 5);
//foreach (new ArrayIterator($a) as $k => $v)
foreach (new LimitIterator(new ArrayIterator($a), 1, 3) as $k => $v)
{
	var_dump(array($k, $v));
}

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "one"
  [1]=>
  int(1)
}
array(2) {
  [0]=>
  string(3) "two"
  [1]=>
  int(2)
}
array(2) {
  [0]=>
  string(5) "three"
  [1]=>
  int(3)
}
===DONE===
