--TEST--
getopt#003
--ARGS--
-vvv --v -a value --another value -1111 -2 --12 --0 --0 --1 -v
--INI--
register_argc_argv=On
variables_order=GPS
--FILE--
<?php
	var_dump(getopt("2a:vcd1", array("another:", 12, 0, 1, "v")));
?>
--EXPECT--
array(7) {
  ["v"]=>
  array(5) {
    [0]=>
    bool(false)
    [1]=>
    bool(false)
    [2]=>
    bool(false)
    [3]=>
    bool(false)
    [4]=>
    bool(false)
  }
  ["a"]=>
  string(5) "value"
  ["another"]=>
  string(5) "value"
  [1]=>
  array(5) {
    [0]=>
    bool(false)
    [1]=>
    bool(false)
    [2]=>
    bool(false)
    [3]=>
    bool(false)
    [4]=>
    bool(false)
  }
  [2]=>
  bool(false)
  [12]=>
  bool(false)
  [0]=>
  array(2) {
    [0]=>
    bool(false)
    [1]=>
    bool(false)
  }
}


