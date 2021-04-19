--TEST--
Partial application variation debug user
--FILE--
<?php
function bar($a = 1, $b = 2, ...$c) {

}

var_dump(bar(?, new stdClass, 20, new stdClass, four: 4));
?>
--EXPECTF--
object(Closure)#%d (2) {
  ["parameter"]=>
  array(1) {
    ["$a"]=>
    string(10) "<required>"
  }
  ["args"]=>
  array(3) {
    ["a"]=>
    NULL
    ["b"]=>
    object(stdClass)#%d (0) {
    }
    ["c"]=>
    array(3) {
      [0]=>
      int(20)
      [1]=>
      object(stdClass)#%d (0) {
      }
      ["four"]=>
      int(4)
    }
  }
}
