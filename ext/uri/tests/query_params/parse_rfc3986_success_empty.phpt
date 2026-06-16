--TEST--
Test Uri\QueryParams::parseRfc3986() - success - empty string
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("");

var_dump($params->list());
var_dump($params->toRfc3986String());

?>
--EXPECT--
array(0) {
}
string(0) ""
