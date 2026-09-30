--TEST--
Test Uri\QueryParams cloning - success - deleted entries and null values
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986("x=1&x=2&flag")->deleteValue("x", "1");
$params2 = clone $params1;

var_dump($params1 === $params2);

unset($params1);
var_dump($params2->toRfc3986String());
var_dump($params2->getFirst("x"));
var_dump($params2->getLast("x"));
var_dump($params2->count());

$params2 = $params2->append("x", "3");
var_dump($params2->toRfc3986String());
var_dump($params2->getLast("x"));
var_dump($params2->count());

?>
--EXPECT--
bool(false)
string(8) "x=2&flag"
string(1) "2"
string(1) "2"
int(2)
string(12) "x=2&flag&x=3"
string(1) "3"
int(3)
