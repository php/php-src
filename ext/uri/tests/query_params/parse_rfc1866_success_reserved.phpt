--TEST--
Test Uri\QueryParams::parseRfc1866() - success - percent-encoded reserved characters
--FILE--
<?php

$params = Uri\QueryParams::parseRfc1866("ab%3ac=%23123&%3Cfoo%3E=b%40r%2f%3F");

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
    string(4) "ab:c"
    [1]=>
    string(4) "#123"
  }
  [1]=>
  array(2) {
    [0]=>
    string(5) "<foo>"
    [1]=>
    string(5) "b@r/?"
  }
}
string(35) "ab%3Ac=%23123&%3Cfoo%3E=b%40r%2F%3F"
string(27) "ab:c=%23123&%3Cfoo%3E=b@r/?"
string(35) "ab%3Ac=%23123&%3Cfoo%3E=b%40r%2F%3F"
