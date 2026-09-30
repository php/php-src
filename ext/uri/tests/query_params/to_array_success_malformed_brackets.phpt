--TEST--
Test Uri\QueryParams::toArray() - success - malformed names are interpreted entirely literally
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("k[", "1")->append("k[", "2")->append("foo[]]", "3")->append("foo[[a]", "4")->append("foo[a]tail", "5")->append("foo][a]", "6")->append("foo[a][", "7");

var_dump($params->toArray());

?>
--EXPECT--
array(6) {
  ["k["]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
  }
  ["foo[]]"]=>
  string(1) "3"
  ["foo[[a]"]=>
  string(1) "4"
  ["foo[a]tail"]=>
  string(1) "5"
  ["foo][a]"]=>
  string(1) "6"
  ["foo[a]["]=>
  string(1) "7"
}
