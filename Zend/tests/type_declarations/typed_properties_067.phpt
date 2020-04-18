--TEST--
Iterable typed properties must be accepted to by-ref array arguments
--FILE--
<?php

$obj = new class {
    public ?iterable $it = null;
};

function arr(?array &$arr) {
    $arr = [1];
}

arr($obj->it);
var_dump($obj->it);
array_shift($obj->it);
var_dump($obj->it);
parse_str("foo=bar", $obj->it);
var_dump($obj->it);
$obj->it = [];
var_dump($obj->it);

?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
array(0) {
}
array(1) {
  ["foo"]=>
  string(3) "bar"
}
array(0) {
}
