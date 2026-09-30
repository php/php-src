--TEST--
Test Uri\QueryParams::parseRfc3986() - success - empty string
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
array(0) {
}
string(0) ""
string(0) ""
string(0) ""
