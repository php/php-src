--TEST--
GH-25147 (mb_parse_str null byte ends parsing prematurely)
--EXTENSIONS--
mbstring
--INI--
arg_separator.input=&
--FILE--
<?php

mb_parse_str("b=\x00", $output);
var_dump(array_map('bin2hex', $output));

mb_parse_str("b=\x00baz", $output);
var_dump(array_map('bin2hex', $output));

mb_parse_str("a=1\x002&b=\x00baz&c=3", $output);
var_dump(array_map('bin2hex', $output));

mb_parse_str("b=%00baz", $output);
var_dump(array_map('bin2hex', $output));

?>
--EXPECT--
array(1) {
  ["b"]=>
  string(2) "00"
}
array(1) {
  ["b"]=>
  string(8) "0062617a"
}
array(3) {
  ["a"]=>
  string(6) "310032"
  ["b"]=>
  string(8) "0062617a"
  ["c"]=>
  string(2) "33"
}
array(1) {
  ["b"]=>
  string(8) "0062617a"
}
