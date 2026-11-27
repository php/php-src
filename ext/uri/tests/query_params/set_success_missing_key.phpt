--TEST--
Test Uri\QueryParams::set() - success - a missing name is appended to the new instance
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986("x=1");
$params2 = $params1->set("flag", null);

var_dump($params1->toRfc3986String());
var_dump($params1->has("flag"));
var_dump($params2->toRfc3986String());
var_dump($params2->has("flag"));

?>
--EXPECT--
string(3) "x=1"
bool(false)
string(8) "x=1&flag"
bool(true)
