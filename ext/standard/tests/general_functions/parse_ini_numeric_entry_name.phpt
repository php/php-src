--TEST--
parse_ini_string with numeric entry name
--FILE--
<?php

var_dump(parse_ini_string("
1[]=1
2M[]=2
"));

?>
--EXPECT--
array(2) {
  [1]=>
  array(1) {
    [0]=>
    string(1) "1"
  }
  ["2M"]=>
  array(1) {
    [0]=>
    string(1) "2"
  }
}
