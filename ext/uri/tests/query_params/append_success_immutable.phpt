--TEST--
Test Uri\QueryParams::append() - success - the original instance is unchanged
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986("x=1&x=2&flag");
$params2 = $params1->append("x", "3");

var_dump($params1 === $params2);
var_dump($params1->toRfc3986String());
var_dump($params1->getLast("x"));
var_dump($params2->toList());
var_dump($params2->toRfc1866String());
var_dump($params2->toRfc3986String());
var_dump($params2->toWhatWgUrlString());

?>
--EXPECT--
bool(false)
string(12) "x=1&x=2&flag"
string(1) "2"
array(4) {
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
  [2]=>
  array(2) {
    [0]=>
    string(4) "flag"
    [1]=>
    NULL
  }
  [3]=>
  array(2) {
    [0]=>
    string(1) "x"
    [1]=>
    string(1) "3"
  }
}
string(16) "x=1&x=2&flag&x=3"
string(16) "x=1&x=2&flag&x=3"
string(16) "x=1&x=2&flag&x=3"
