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

echo "\nTest with duplicated param name should not be treated as array\n";
$str = "foo[]=sid&foo=bill";
var_dump(parse_str($str, $res));
var_dump($res);

echo "\nTest with duplicated mixed param name should be treated as array\n";
$str = "foo[]=sid&foo=bill";
var_dump(parse_str($str, $res, true));
var_dump($res);

echo "\nTest with duplicated mixed param name should be treated as array (reverse)\n";
$str = "foo=sid&foo[]=bill";
var_dump(parse_str($str, $res, true));
var_dump($res);

echo "\nTest with more duplicated mixed param name should be treated as array\n";
$str = "foo=sid&foo[]=bill&foo=bar&foo[]=baz";
var_dump(parse_str($str, $res, true));
var_dump($res);

echo "\nTest with more duplicated mixed param name should be treated as array 2\n";
$str = "foo[]=sid&foo=bill&foo[]=bar&foo[]=baz";
var_dump(parse_str($str, $res, true));
var_dump($res);

echo "\nTest with 2 different array vars\n";
$str = "foo[]=sid&foo=bill&bar=bar&bar[]=baz";
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

Test with duplicated param name should not be treated as array
NULL
array(1) {
  ["foo"]=>
  string(4) "bill"
}

Test with duplicated mixed param name should be treated as array
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

Test with duplicated mixed param name should be treated as array (reverse)
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

Test with more duplicated mixed param name should be treated as array
NULL
array(1) {
  ["foo"]=>
  array(4) {
    [0]=>
    string(3) "sid"
    [1]=>
    string(4) "bill"
    [2]=>
    string(3) "bar"
    [3]=>
    string(3) "baz"
  }
}

Test with more duplicated mixed param name should be treated as array 2
NULL
array(1) {
  ["foo"]=>
  array(4) {
    [0]=>
    string(3) "sid"
    [1]=>
    string(4) "bill"
    [2]=>
    string(3) "bar"
    [3]=>
    string(3) "baz"
  }
}

Test with 2 different array vars
NULL
array(2) {
  ["foo"]=>
  array(2) {
    [0]=>
    string(3) "sid"
    [1]=>
    string(4) "bill"
  }
  ["bar"]=>
  array(2) {
    [0]=>
    string(3) "bar"
    [1]=>
    string(3) "baz"
  }
}
