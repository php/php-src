--TEST--
Test Uri\QueryParams::has() - success - non-existent
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&foo=bar");

var_dump($params->has("bar"));

?>
--EXPECT--
bool(false)
