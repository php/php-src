--TEST--
Test Uri\QueryParams::append() - success - list
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("foo=0");
$params->append("foo", [1, 2, 3]);

var_dump($params->list());
var_dump($params->toRfc3986String());

?>
--EXPECTF--
object(Uri\QueryParams)#%d (%d) {
  ["params"]=>
  array(4) {
    [0]=>
    array(2) {
      [0]=>
      string(3) "foo"
      [1]=>
      string(1) "0"
    }
    [1]=>
    array(2) {
      [0]=>
      string(5) "foo[]"
      [1]=>
      string(1) "1"
    }
    [2]=>
    array(2) {
      [0]=>
      string(5) "foo[]"
      [1]=>
      string(1) "2"
    }
    [3]=>
    array(2) {
      [0]=>
      string(5) "foo[]"
      [1]=>
      string(1) "3"
    }
  }
}
string(41) "foo=0&foo%5B%5D=1&foo%5B%5D=2&foo%5B%5D=3"
