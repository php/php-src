--TEST--
Test Uri\QueryParams::append() - success - multi-level array value
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("");
$params->append("foo", [[0 => "bar"], [123]]);

var_dump($params->list());
var_dump($params->toRfc3986String());

?>
--EXPECTF--
object(Uri\QueryParams)#%d (%d) {
  ["foo[][]"]=>
  string(3) "bar"
  ["foo[][]"]=>
  string(3) "123"
}
string(39) "foo%5B%5D%5B%5D=bar&foo%5B%5D%5B%5D=123"
