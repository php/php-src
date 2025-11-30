--TEST--
Cast operators with complex expressions
--FILE--
<?php

var_dump((?int) (1 + 2));
var_dump((!int) (3 * 4));

function getValue() {
    return "123";
}

var_dump((?int) getValue());
var_dump((!int) getValue());

$x = null;
$result = ((?int) $x) ?? 10;
var_dump($result);

$arr = [
    (?int) null,
    (?int) "789",
    (!string) 123
];
var_dump($arr);

?>
--EXPECT--
int(3)
int(12)
int(123)
int(123)
int(10)
array(3) {
  [0]=>
  NULL
  [1]=>
  int(789)
  [2]=>
  string(3) "123"
}
