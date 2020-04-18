--TEST--
Closure 033: Recursive var_dump on closures
--FILE--
<?php

$a = function () use(&$a) {};
var_dump($a);

?>
--EXPECTF--
object(Closure)#%d (1) {
  ["static"]=>
  array(1) {
    ["a"]=>
    *RECURSION*
  }
}
