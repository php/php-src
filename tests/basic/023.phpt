--TEST--
Cookies test#2
--INI--
max_input_vars=1000
filter.default=unsafe_raw
--COOKIE--
c o o k i e=value; c o o k i e= v a l u e ;;c%20o+o k+i%20e=v;name="value","value",UEhQIQ==;UEhQIQ==foo
--FILE--
<?php
var_dump($_COOKIE);
?>
--EXPECT--
array(3) {
  ["c_o_o_k_i_e"]=>
  string(5) "value"
  ["name"]=>
  string(24) ""value","value",UEhQIQ=="
  ["UEhQIQ"]=>
  string(4) "=foo"
}
