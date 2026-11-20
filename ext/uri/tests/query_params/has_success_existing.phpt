--TEST--
Test Uri\QueryParams::has() - success - existing
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("abc=123&foo=bar");

var_dump($params->has("abc"));

?>
--EXPECT--
bool(true)
