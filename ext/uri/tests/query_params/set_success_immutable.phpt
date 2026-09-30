--TEST--
Test Uri\QueryParams::set() - success - the original instance is unchanged
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986("x=1&x=2&flag");
$params2 = $params1->set("x", 3);

var_dump($params1 === $params2);
var_dump($params1->toRfc3986String());
var_dump($params1->getFirst("x"));
var_dump($params1->getLast("x"));
var_dump($params2->toRfc3986String());
var_dump($params2->getFirst("x"));
var_dump($params2->getLast("x"));
var_dump($params2->count());

?>
--EXPECT--
bool(false)
string(12) "x=1&x=2&flag"
string(1) "1"
string(1) "2"
string(8) "x=3&flag"
string(1) "3"
string(1) "3"
int(2)
