--TEST--
Test Uri\QueryParams::parseRfc3986() - success - reserved characters are not validated
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("ab:c=#123&<foo>=b@r");

var_dump($params->list());
var_dump($params->toRfc3986String());

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
    string(3) "b@r"
  }
}
string(25) "ab:c=%23123&%3Cfoo%3E=b@r"
