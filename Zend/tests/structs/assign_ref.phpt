--TEST--
Assign ref on structs
--FILE--
<?php

struct Box {
    public $value;
}

$a = new Box();
$a->value = 1;
$b = $a;
$c = 2;
$b->value = &$c;
var_dump($a);
var_dump($b);

?>
--EXPECT--
object(Box)#1 (1) {
  ["value"]=>
  int(1)
}
object(Box)#2 (1) {
  ["value"]=>
  &int(2)
}
