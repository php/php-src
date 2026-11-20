--TEST--
Test Uri\QueryParams::delete() - success - basic
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&foo=bar");
$params = $params->delete("abc");
$params = $params->delete("foo");

var_dump($params->toList());
var_dump($params->toRfc3986String());

?>
--EXPECT--
array(0) {
}
string(0) ""
