--TEST--
PFA variation: var_dump(), internal function
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--ENV--
A=1
--FILE--
<?php

function notconst($value) {
    return getenv('A') ? $value : null;
}

var_dump(array_map(?, notconst([1, 2, 3]), notconst([4, 5, 6]), four: new stdClass, ...));
?>
--EXPECTF--
object(Closure)#%d (5) {
  ["name"]=>
  string(%d) "{closure:%s}"
  ["file"]=>
  string(%d) "%svariation_debug_002.php"
  ["line"]=>
  int(7)
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
    ["arrays2"]=>
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
