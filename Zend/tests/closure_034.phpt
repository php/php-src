--TEST--
Closure 033: Recursive var_dump on closures
--FILE--
<?php

$a = function () use(&$a) {};
var_dump($a);

?>
===DONE===
--EXPECTF--
object(Closure)#1 (2) {
  ["this"]=>
  NULL
  ["static"]=>
  array(1) {
    [u"a"]=>
    &object(Closure)#1 (2) {
      ["this"]=>
      NULL
      ["static"]=>
      array(1) {
        [u"a"]=>
        *RECURSION*
      }
    }
  }
}
===DONE===
