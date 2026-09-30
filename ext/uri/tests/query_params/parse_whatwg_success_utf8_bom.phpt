--TEST--
Test Uri\QueryParams::parseWhatWgUrlSearchParams() - success - UTF-8 bom
--FILE--
<?php

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("%EF%BB%BFa=%EF%BB%BFb");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECTF--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(4) "%r\xEF\xBB\xBF\x61%r"
    [1]=>
    string(4) "%r\xEF\xBB\xBF\x62%r"
  }
}
string(21) "%EF%BB%BFa=%EF%BB%BFb"
string(21) "%EF%BB%BFa=%EF%BB%BFb"
string(21) "%EF%BB%BFa=%EF%BB%BFb"
