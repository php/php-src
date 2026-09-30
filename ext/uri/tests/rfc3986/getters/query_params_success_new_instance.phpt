--TEST--
Test Uri\Rfc3986\Uri::getQueryParams() - success - each call creates an independent instance
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://example.com?x=1");

$first = $uri->getQueryParams();
$params = $uri->getQueryParams();
var_dump($first === $params);

$first = $first->append("y", "2");
var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());
var_dump($uri->getRawQuery());

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
