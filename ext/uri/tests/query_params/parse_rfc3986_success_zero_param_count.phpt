--TEST--
Test Uri\QueryParams::parseRfc3986() - success - zero param count disables the limit
--FILE--
<?php

$query = implode("&", array_fill(0, 2000, "x=1"));

$params = Uri\QueryParams::parseRfc3986($query, maxParamCount: 0);

var_dump($params->toList() === array_fill(0, 2000, ["x", "1"]));
var_dump($params->toRfc1866String() === $query);
var_dump($params->toRfc3986String() === $query);
var_dump($params->toWhatWgUrlString() === $query);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
