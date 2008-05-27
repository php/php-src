--TEST--
getopt#004 (Optional values)
--ARGS--
-v -v1 -v=10 --v --v=100
--INI--
register_argc_argv=On
variables_order=GPS
--FILE--
<?php
	var_dump(getopt("v::", array("v::")));
?>
--EXPECT--
array(1) {
  ["v"]=>
  array(5) {
    [0]=>
    bool(false)
    [1]=>
    string(1) "1"
    [2]=>
    string(2) "10"
    [3]=>
    bool(false)
    [4]=>
    string(3) "100"
  }
}
