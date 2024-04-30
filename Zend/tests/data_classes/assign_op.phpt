--TEST--
Assign op on data classes
--FILE--
<?php

data class Box {
    public function __construct(
        public $value,
    ) {}
}

$a = new Box(0);
$b = $a;
$b->value += 1;
var_dump($a);
var_dump($b);

?>
--EXPECT--
object(Box)#1 (1) {
  ["value"]=>
  int(0)
}
object(Box)#2 (1) {
  ["value"]=>
  int(1)
}
