--TEST--
Using get_cfg_var() on an array ini value
--INI--
ary[a] = 1
ary[b] = 2
ary2[1] = a
ary2[2] = b
--FILE--
<?php

var_dump(get_cfg_var('ary'));
var_dump(get_cfg_var('ary2'));

?>
--EXPECT--
array(2) {
  ["a"]=>
  string(1) "1"
  ["b"]=>
  string(1) "2"
}
array(2) {
  [1]=>
  string(1) "a"
  [2]=>
  string(1) "b"
}
