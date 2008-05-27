--TEST--
getopt#005 (Required values)
--ARGS--
--arg value --arg=value -avalue -a=value -a value
--INI--
register_argc_argv=On
variables_order=GPS
--FILE--
<?php
	var_dump(getopt("a:", array("arg:")));
?>
--EXPECT--
array(2) {
  ["arg"]=>
  array(2) {
    [0]=>
    string(5) "value"
    [1]=>
    string(5) "value"
  }
  ["a"]=>
  array(3) {
    [0]=>
    string(5) "value"
    [1]=>
    string(5) "value"
    [2]=>
    string(5) "value"
  }
}
