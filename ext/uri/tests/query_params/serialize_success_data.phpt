--TEST--
Test Uri\QueryParams::__serialize() - success - independent list snapshot and empty properties
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986("x=1&x");
$data = $params1->__serialize();
$data[0]["params"][0][1] = "changed";

var_dump($params1->toRfc3986String());

$data = $params1->__serialize();
unset($params1);
var_dump($data[0]);
var_dump($data[1]);

?>
--EXPECT--
string(5) "x=1&x"
array(1) {
  ["params"]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      string(1) "x"
      [1]=>
      string(1) "1"
    }
    [1]=>
    array(2) {
      [0]=>
      string(1) "x"
      [1]=>
      NULL
    }
  }
}
array(0) {
}
