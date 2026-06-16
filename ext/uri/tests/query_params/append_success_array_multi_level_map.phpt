--TEST--
Test Uri\QueryParams::append() - success - multi-level array value
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("");
$params->append("foo", ["bar" => [0 => "baz", 2 => 123, 3 => [], 4 => [1, 2, 3]]]);

var_dump($params->list());
var_dump($params->toRfc3986String());

?>
--EXPECTF--
object(Uri\QueryParams)#%d (%d) {
  ["params"]=>
  array(6) {
    [0]=>
    array(2) {
      [0]=>
      string(1) "0"
      [1]=>
      string(3) "baz"
    }
    [1]=>
    array(2) {
      [0]=>
      string(11) "foo[bar][2]"
      [1]=>
      string(3) "123"
    }
    [2]=>
    array(2) {
      [0]=>
      string(11) "foo[bar][3]"
      [1]=>
      NULL
    }
    [3]=>
    array(2) {
      [0]=>
      string(13) "foo[bar][4][]"
      [1]=>
      string(1) "1"
    }
    [4]=>
    array(2) {
      [0]=>
      string(13) "foo[bar][4][]"
      [1]=>
      string(1) "2"
    }
    [5]=>
    array(2) {
      [0]=>
      string(13) "foo[bar][4][]"
      [1]=>
      string(1) "3"
    }
  }
}
string(133) "0=baz&foo%5Bbar%5D%5B2%5D=123&foo%5Bbar%5D%5B3%5D&foo%5Bbar%5D%5B4%5D%5B%5D=1&foo%5Bbar%5D%5B4%5D%5B%5D=2&foo%5Bbar%5D%5B4%5D%5B%5D=3"
