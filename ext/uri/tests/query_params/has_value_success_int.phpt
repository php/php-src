--TEST--
Test Uri\QueryParams::hasValue() - success - int value
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&foo=bar");

var_dump($params->hasValue("abc", "123"));

?>
--EXPECT--
bool(true)
