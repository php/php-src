--TEST--
PFA variation: var_dump(), internal function
--FILE--
<?php
var_dump(array_map(?, [1, 2, 3], [4, 5, 6], four: new stdClass, ...));
?>
--EXPECTF--
object(Closure)#%d (5) {
  ["name"]=>
  string(%d) "{closure:%s}"
  ["file"]=>
  string(%d) "%svariation_debug_002.php"
  ["line"]=>
  int(2)
  ["static"]=>
  array(3) {
    ["array"]=>
    array(3) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
    }
    ["arrays0"]=>
    array(3) {
      [0]=>
      int(4)
      [1]=>
      int(5)
      [2]=>
      int(6)
    }
    ["extra_named_params"]=>
    array(1) {
      ["four"]=>
      object(stdClass)#%d (0) {
      }
    }
  }
  ["parameter"]=>
  array(2) {
    ["$callback"]=>
    string(10) "<required>"
    ["$arrays"]=>
    string(10) "<optional>"
  }
}
