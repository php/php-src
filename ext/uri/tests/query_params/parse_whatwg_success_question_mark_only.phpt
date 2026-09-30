--TEST--
Test Uri\QueryParams::parseWhatWgUrlSearchParams() - success - a lone question mark produces an empty list
--FILE--
<?php

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("?");

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
