--TEST--
PFA variation: var_dump(), user function
--FILE--
<?php
function bar($a = 1, $b = 2, ...$c) {

}

var_dump(bar(?, new stdClass, 20, new stdClass, four: 4));
?>
--EXPECTF--
object(Closure)#%d (5) {
  ["name"]=>
  string(%d) "{closure:%s}"
  ["file"]=>
  string(%d) "%svariation_debug_001.php"
  ["line"]=>
  int(6)
  ["static"]=>
  array(4) {
    ["b"]=>
    object(stdClass)#%d (0) {
    }
    ["c"]=>
    int(20)
    ["c0"]=>
    object(stdClass)#%d (0) {
    }
    ["extra_named_params"]=>
    array(1) {
      ["four"]=>
      int(4)
    }
  }
  ["parameter"]=>
  array(1) {
    ["$a"]=>
    string(10) "<optional>"
  }
}
