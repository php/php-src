--TEST--
Constructor promotion of by-ref parameter
--FILE--
<?php

class Ary {
    public function __construct(public array &$array) {}
}

$array = [];
$ary = new Ary($array);
$array[] = 42;
var_dump($ary->array);

?>
--EXPECT--
array(1) {
  [0]=>
  int(42)
}
