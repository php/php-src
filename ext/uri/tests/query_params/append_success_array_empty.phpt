--TEST--
Test Uri\QueryParams::append() - success - empty array value
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("foo=bar");
$params->append("baz", []);

var_dump($params->list());
var_dump($params->toRfc3986String());

?>
--EXPECTF--
object(Uri\QueryParams)#%d (%d) {
  ["params"]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      string(3) "foo"
      [1]=>
      string(3) "bar"
    }
    [1]=>
    array(2) {
      [0]=>
      string(3) "baz"
      [1]=>
      NULL
    }
  }
}
string(11) "foo=bar&baz"
