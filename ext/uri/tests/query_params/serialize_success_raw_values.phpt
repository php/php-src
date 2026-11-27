--TEST--
Test Uri\QueryParams serialization - success - preserve empty fields and arbitrary bytes
--FILE--
<?php

$params1 = (new Uri\QueryParams())->append("", null)->append("", "")->append("a\0b%FF", "\xFF\0%20\r\n");
$params2 = unserialize(serialize($params1));

var_dump($params2->toList() === $params1->toList());
var_dump($params2->getAll(""));
var_dump(bin2hex($params2->getFirst("a\0b%FF")));
var_dump($params2->count());

?>
--EXPECT--
bool(true)
array(2) {
  [0]=>
  NULL
  [1]=>
  string(0) ""
}
string(14) "ff002532300d0a"
int(3)
