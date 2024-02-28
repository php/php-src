--TEST--
Closure 033: Recursive var_dump on closures
--FILE--
<?php

$a = function () use(&$a) {};
var_dump($a);

?>
--EXPECTF--
object(Closure)#%d (2) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["static"]=>
  array(1) {
    ["a"]=>
    *RECURSION*
  }
}
