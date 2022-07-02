--TEST--
Cookies test#1
--INI--
max_input_vars=1000
filter.default=unsafe_raw
--COOKIE--
cookie1=val1  ; cookie2=val2%20; cookie3=val 3.; cookie 4= value 4 %3B; cookie1=bogus; %20cookie1=ignore;+cookie1=ignore;cookie1;cookie  5=%20 value; cookie%206=þæö;cookie+7=;$cookie.8;cookie-9=1;;;- & % $cookie 10=10
--FILE--
<?php
var_dump($_COOKIE);
?>
--EXPECT--
array(12) {
  ["cookie1"]=>
  string(6) "val1  "
  ["cookie2"]=>
  string(5) "val2 "
  ["cookie3"]=>
  string(6) "val 3."
  ["cookie_4"]=>
  string(10) " value 4 ;"
  ["%20cookie1"]=>
  string(6) "ignore"
  ["+cookie1"]=>
  string(6) "ignore"
  ["cookie__5"]=>
  string(7) "  value"
  ["cookie%206"]=>
  string(3) "þæö"
  ["cookie+7"]=>
  string(0) ""
  ["$cookie_8"]=>
  string(0) ""
  ["cookie-9"]=>
  string(1) "1"
  ["-_&_%_$cookie_10"]=>
  string(2) "10"
}
