--TEST--
Test Uri\QueryParams::hasValue() - success - missing key
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&foo=bar");

var_dump($params->hasValue("baz", "abc"));

?>
--EXPECT--
bool(false)
