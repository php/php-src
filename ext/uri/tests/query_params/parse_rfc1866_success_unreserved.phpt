--TEST--
Test Uri\QueryParams::parseRfc1866() - success - unreserved characters remain literal
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("AZaz09-._~=AZaz09-._~");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(10) "AZaz09-._~"
    [1]=>
    string(10) "AZaz09-._~"
  }
}
string(21) "AZaz09-._~=AZaz09-._~"
string(21) "AZaz09-._~=AZaz09-._~"
string(25) "AZaz09-._%7E=AZaz09-._%7E"
