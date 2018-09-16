--TEST--
filter_var()
--INI--
precision=14
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

var_dump(filter_var(array(1,"1","", "-23234", "text", "asdf234asdfgs", array()), FILTER_VALIDATE_INT, FILTER_REQUIRE_ARRAY));
var_dump(filter_var(array(1.2,"1.7","", "-23234.123", "text", "asdf234.2asdfgs", array()), FILTER_VALIDATE_FLOAT, FILTER_REQUIRE_ARRAY));
var_dump(filter_var(1, array()));
var_dump(filter_var(1, FILTER_SANITIZE_STRING, 1));
var_dump(filter_var(1, FILTER_SANITIZE_STRING, 0));
var_dump(filter_var(1, FILTER_SANITIZE_STRING, array()));
var_dump(filter_var(1, -1, array(123)));
var_dump(filter_var(1, 0, array()));

echo "Done\n";
?>
--EXPECTF--
array(7) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  bool(false)
  [3]=>
  int(-23234)
  [4]=>
  bool(false)
  [5]=>
  bool(false)
  [6]=>
  array(0) {
  }
}
array(7) {
  [0]=>
  float(1.2)
  [1]=>
  float(1.7)
  [2]=>
  bool(false)
  [3]=>
  float(-23234.123)
  [4]=>
  bool(false)
  [5]=>
  bool(false)
  [6]=>
  array(0) {
  }
}

Warning: filter_var() expects parameter 2 to be int, array given in %s on line %d
NULL
string(1) "1"
string(1) "1"
string(1) "1"
bool(false)
bool(false)
Done
