--TEST--
Test Uri\QueryParams::append() - success - a temporary instance used as a WeakMap key is cloned
--FILE--
<?php

function createParams(WeakMap $map): Uri\QueryParams {
    $params = Uri\QueryParams::parseRfc3986("x=1");
    $map[$params] = true;
    return $params;
}

$map = new WeakMap();
$params = createParams($map)->append("x", "2");

var_dump(count($map));
var_dump(isset($map[$params]));
var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
int(0)
bool(false)
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
