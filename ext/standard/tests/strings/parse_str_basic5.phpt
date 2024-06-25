--TEST--
Test parse_str() function : test with duplicated param name
--FILE--
<?php
echo "\nTest with duplicated param name should be treated as single var with latest value set\n";
$str = "foo=sid&foo=bill";
var_dump(parse_str($str, $res));
var_dump($res);

echo "\nTest with duplicated param name should be treated as array\n";
$str = "foo=sid&foo=bill";
var_dump(parse_str($str, $res, true));
var_dump($res);
?>
--EXPECTF--
Test with duplicated param name should be treated as single var with latest value set
NULL
array(1) {
  ["foo"]=>
  string(4) "bill"
}

Test with duplicated param name should be treated as array
NULL
array(1) {
  ["foo"]=>
  array(2) {
    [0]=>
    string(3) "sid"
    [1]=>
    string(4) "bill"
  }
}
