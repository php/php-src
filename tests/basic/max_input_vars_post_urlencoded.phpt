--TEST--
max_input_vars limit for urlencoded POST data
--INI--
max_input_vars=5
--POST--
a=1&b=2&c=3&d=4&e=5&f=6
--FILE--
<?php
var_dump($_POST);
?>
--EXPECT--
Warning: PHP Request Startup: Input variables exceeded 5. To increase the limit change max_input_vars in php.ini. in Unknown on line 0
array(5) {
  ["a"]=>
  string(1) "1"
  ["b"]=>
  string(1) "2"
  ["c"]=>
  string(1) "3"
  ["d"]=>
  string(1) "4"
  ["e"]=>
  string(1) "5"
}
