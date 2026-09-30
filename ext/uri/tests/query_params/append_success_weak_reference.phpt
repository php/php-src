--TEST--
Test Uri\QueryParams::append() - success - a weakly referenced temporary instance is cloned
--FILE--
<?php

function createParams(&$reference): Uri\QueryParams {
    $params = Uri\QueryParams::parseRfc3986("x=1");
    $reference = WeakReference::create($params);
    return $params;
}

$params = createParams($reference)->append("x", "2");

var_dump($reference->get());
var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
NULL
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "x"
    [1]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "x"
    [1]=>
    string(1) "2"
  }
}
string(7) "x=1&x=2"
string(7) "x=1&x=2"
string(7) "x=1&x=2"
