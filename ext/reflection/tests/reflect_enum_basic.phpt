--TEST--
Basic reflect_enum() test
--FILE--
<?php

enum a { d, f }
enum b { c, e }

var_dump(reflect_enum(a::f));

?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "f"
}
