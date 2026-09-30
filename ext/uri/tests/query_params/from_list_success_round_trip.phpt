--TEST--
Test Uri\QueryParams::fromList() - success - toList() round trip preserves order, duplicate names and raw bytes
--FILE--
<?php

$params1 = (new Uri\QueryParams())
    ->append("tag", "first")
    ->append("", null)
    ->append("tag", "second")
    ->append("", "")
    ->append("a[]\0", "\xFF%20");
$params2 = Uri\QueryParams::fromList($params1->toList());

var_dump($params2->toList() === $params1->toList());
var_dump($params2->getAll("tag"));
var_dump($params2->getAll(""));
var_dump(bin2hex($params2->getFirst("a[]\0")));

?>
--EXPECT--
bool(true)
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
array(2) {
  [0]=>
  NULL
  [1]=>
  string(0) ""
}
string(8) "ff253230"
