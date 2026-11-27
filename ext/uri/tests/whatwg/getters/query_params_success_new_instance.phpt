--TEST--
Test Uri\WhatWg\Url::getQueryParams() - success - each call creates an independent instance
--FILE--
<?php

$uri = new Uri\WhatWg\Url("https://example.com?x=1");

$first = $uri->getQueryParams();
$params = $uri->getQueryParams();
var_dump($first === $params);

$first = $first->append("y", "2");
var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());
var_dump($uri->getQuery());

?>
--EXPECT--
bool(false)
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "x"
    [1]=>
    string(1) "1"
  }
}
string(3) "x=1"
string(3) "x=1"
string(3) "x=1"
string(3) "x=1"
