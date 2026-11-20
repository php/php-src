--TEST--
Test Uri\QueryParams::append() - success - an unshared temporary instance is reused
--FILE--
<?php

function createParams(&$id): Uri\QueryParams {
    $params = Uri\QueryParams::parseRfc3986("x=1");
    $id = spl_object_id($params);
    return $params;
}

$params = createParams($id)->append("x", "2");

var_dump(spl_object_id($params) === $id);
var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
bool(true)
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
