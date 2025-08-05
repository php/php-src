--TEST--
clone() can be partially applied
--FILE--
<?php

class C {
    public function __construct(
        public mixed $a,
        public mixed $b,
    ) { }
}

$clone = clone(?);
var_dump($clone(new C(1, 2)));

$clone = clone(...);
var_dump($clone(new C(3, 4)));

$clone = clone(new C(5, 6), ?);
var_dump($clone(['a' => 7]));

$clone = clone(?, ['a' => 8]);
var_dump($clone(new C(9, 10)));

?>
--EXPECTF--
object(C)#%d (2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
object(C)#%d (2) {
  ["a"]=>
  int(3)
  ["b"]=>
  int(4)
}
object(C)#%d (2) {
  ["a"]=>
  int(7)
  ["b"]=>
  int(6)
}
object(C)#%d (2) {
  ["a"]=>
  int(8)
  ["b"]=>
  int(10)
}
