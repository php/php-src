--TEST--
filter_data() and FILTER_VALIDATE_BOOLEAN
--FILE--
<?php

class test {

	function __toString() {
		return "blah";
	}
}

$t = new test;

var_dump(filter_data("no", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data(NULL, FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data($t, FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data(array(1,2,3,0,array("", "123")), FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("yes", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("true", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("false", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("off", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("on", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("0", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("1", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("NONE", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data(-1, FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("000000", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("111111", FILTER_VALIDATE_BOOLEAN));
	

echo "Done\n";
?>
--EXPECTF--	
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
