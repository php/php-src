--TEST--
Test Uri\QueryParams::hasValue() - success - existing key, existing value
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&foo=bar");

var_dump($params->hasValue("foo", "bar"));

?>
--EXPECT--
bool(true)
