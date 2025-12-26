--TEST--
Send structs
--FILE--
<?php

struct Box {
    public function __construct(
        public $value,
    ) {}
}

$a = new Box(42);
$a->value = $a;
var_dump($a);

$b = new Box(42);
$b->value = &$b;
var_dump($b);

?>
--EXPECT--
object(Box)#2 (1) {
  ["value"]=>
  object(Box)#1 (1) {
    ["value"]=>
    int(42)
  }
}
object(Box)#3 (1) {
  ["value"]=>
  *RECURSION*
}
