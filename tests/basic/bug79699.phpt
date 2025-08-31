--TEST--
Cookies Security Bug
--INI--
max_input_vars=1000
filter.default=unsafe_raw
--COOKIE--
__%48ost-evil=evil; __Host-evil=good; %66oo=baz;foo=bar
--FILE--
<?php
var_dump($_COOKIE);
?>
--EXPECT--
array(4) {
  ["__%48ost-evil"]=>
  string(4) "evil"
  ["__Host-evil"]=>
  string(4) "good"
  ["%66oo"]=>
  string(3) "baz"
  ["foo"]=>
  string(3) "bar"
}
