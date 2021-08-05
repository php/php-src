--TEST--
Test parse_query_string() function: non-default arg_separator.input specified
--INI--
arg_separator.input = "/"
--FILE--
<?php

$s1 = "first=val1/second=val2/third=val3";
var_dump(parse_query_string($s1));

?>
--EXPECT--
array(3) {
  ["first"]=>
  string(4) "val1"
  ["second"]=>
  string(4) "val2"
  ["third"]=>
  string(4) "val3"
}