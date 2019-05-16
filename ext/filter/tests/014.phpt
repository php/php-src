--TEST--
filter_var() and FILTER_VALIDATE_BOOLEAN
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

class test {

	function __toString() {
		return "blah";
	}
}

$t = new test;

var_dump(filter_var("no", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var(NULL, FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var($t, FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var(array(1,2,3,0,array("", "123")), FILTER_VALIDATE_BOOLEAN, FILTER_REQUIRE_ARRAY));
var_dump(filter_var("yes", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var("true", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var("false", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var("off", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var("on", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var("0", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var("1", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var("NONE", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var("", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var(-1, FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var("000000", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_var("111111", FILTER_VALIDATE_BOOLEAN));


echo "Done\n";
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
array(5) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
  [2]=>
  bool(false)
  [3]=>
  bool(false)
  [4]=>
  array(2) {
    [0]=>
    bool(false)
    [1]=>
    bool(false)
  }
}
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Done
