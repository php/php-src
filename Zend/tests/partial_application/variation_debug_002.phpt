--TEST--
Partial application variation debug internal
--FILE--
<?php
var_dump(array_map(?, [1,2,3], [4,5, 6], ..., four: new stdClass));
?>
--EXPECTF--
object(Closure)#%d (2) {
  ["parameter"]=>
  array(2) {
    ["$callback"]=>
    string(10) "<required>"
    ["$arrays"]=>
    string(10) "<optional>"
  }
  ["args"]=>
  array(3) {
    ["callback"]=>
    NULL
    ["array"]=>
    array(3) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
    }
    ["arrays"]=>
    array(3) {
      [0]=>
      array(3) {
        [0]=>
        int(4)
        [1]=>
        int(5)
        [2]=>
        int(6)
      }
      [1]=>
      NULL
      ["four"]=>
      object(stdClass)#%d (0) {
      }
    }
  }
}

