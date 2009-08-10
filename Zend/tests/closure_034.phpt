--TEST--
Closure 033: Recursive var_dump on closures
--FILE--
<?php

$a = function () use(&$a) {};
var_dump($a);

?>
===DONE===
--EXPECTF--
object(Closure)#1 (1) {
  ["static"]=>
  array(1) {
    ["a"]=>
    &object(Closure)#1 (1) {
      ["static"]=>
      array(1) {
        ["a"]=>
        *RECURSION*
      }
    }
  }
}
===DONE===
