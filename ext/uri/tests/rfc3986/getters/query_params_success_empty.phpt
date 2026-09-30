--TEST--
Test Uri\Rfc3986\Uri::getQueryParams() - success - a present empty query returns an empty instance
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://example.com?");

$params = $uri->getQueryParams();

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
