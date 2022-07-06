--TEST--
getopt#002
--ARGS--
-vvv -a value -1111 -2 -v
--INI--
register_argc_argv=On
variables_order=GPS
--FILE--
<?php
    var_dump(getopt("2a:vcd1"));
?>
--EXPECT--
array(4) {
  ["v"]=>
  array(4) {
    [0]=>
    bool(false)
    [1]=>
    bool(false)
    [2]=>
    bool(false)
    [3]=>
    bool(false)
  }
  ["a"]=>
  string(5) "value"
  [1]=>
  array(4) {
    [0]=>
    bool(false)
    [1]=>
    bool(false)
    [2]=>
    bool(false)
    [3]=>
    bool(false)
  }
  [2]=>
  bool(false)
}
