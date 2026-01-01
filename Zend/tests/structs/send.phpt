--TEST--
Send structs
--FILE--
<?php

struct Box {
    public $value;
}

function byVal($box) {
    $box->value = 2;
    var_dump($box);
}

function byRef(&$box) {
    $box->value = 2;
    var_dump($box);
}

$box = new Box();
$box->value = 1;

byVal($box);
var_dump($box);

byRef($box);
var_dump($box);

?>
--EXPECT--
object(Box)#2 (1) {
  ["value"]=>
  int(2)
}
object(Box)#1 (1) {
  ["value"]=>
  int(1)
}
object(Box)#1 (1) {
  ["value"]=>
  int(2)
}
object(Box)#1 (1) {
  ["value"]=>
  int(2)
}
