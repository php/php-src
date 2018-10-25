--TEST--
Bug #43293 (Multiple segfaults in getopt())
--ARGS--
-f --f
--INI--
register_argc_argv=On
--FILE--
<?php
$args = array(true, false, "f");
var_dump(getopt("f", $args), $args);
?>
--EXPECT--
array(1) {
  ["f"]=>
  array(2) {
    [0]=>
    bool(false)
    [1]=>
    bool(false)
  }
}
array(3) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
  [2]=>
  string(1) "f"
}
