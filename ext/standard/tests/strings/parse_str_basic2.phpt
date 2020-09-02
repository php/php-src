--TEST--
Test parse_str() function : non-default arg_separator.input specified
--INI--
arg_separator.input = "/"
--FILE--
<?php
echo "*** Testing parse_str() : non-default arg_separator.input specified ***\n";

$s1 = "first=val1/second=val2/third=val3";
var_dump(parse_str($s1, $result));
var_dump($result);

?>
--EXPECT--
*** Testing parse_str() : non-default arg_separator.input specified ***
NULL
array(3) {
  ["first"]=>
  string(4) "val1"
  ["second"]=>
  string(4) "val2"
  ["third"]=>
  string(4) "val3"
}
