--TEST--
Test Uri\QueryParams::fromArray() - success - names and values are not percent-decoded
--FILE--
<?php

$params = Uri\QueryParams::fromArray(["foo%5B%5D" => "a%20b", "space name" => "a b"]);

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(9) "foo%5B%5D"
    [1]=>
    string(5) "a%20b"
  }
  [1]=>
  array(2) {
    [0]=>
    string(10) "space name"
    [1]=>
    string(3) "a b"
  }
}
string(36) "foo%255B%255D=a%2520b&space+name=a+b"
string(40) "foo%255B%255D=a%2520b&space%20name=a%20b"
string(36) "foo%255B%255D=a%2520b&space+name=a+b"
