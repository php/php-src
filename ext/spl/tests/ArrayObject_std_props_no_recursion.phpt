--TEST--
Don't recurse into USE_OTHER when checking for STD_PROP_LIST
--FILE--
<?php

$a = new ArrayObject([1, 2, 3], ArrayObject::STD_PROP_LIST);
$a->prop = 'a';
$b = new ArrayObject($a, 0);
$b->prop = 'b';
var_dump((array) $b);
$c = new ArrayObject($a);
$c->prop = 'c';
var_dump((array) $c);

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(1) {
  ["prop"]=>
  string(1) "c"
}
