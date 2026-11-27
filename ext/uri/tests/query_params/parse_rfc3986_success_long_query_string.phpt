--TEST--
Test Uri\QueryParams::parseRfc3986() - success - long query string
--FILE--
<?php

$value = str_repeat("a", 20000);
$query = "x=" . $value;

$params = Uri\QueryParams::parseRfc3986($query);

var_dump($params->toList() === [["x", $value]]);
var_dump($params->toRfc1866String() === $query);
var_dump($params->toRfc3986String() === $query);
var_dump($params->toWhatWgUrlString() === $query);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
