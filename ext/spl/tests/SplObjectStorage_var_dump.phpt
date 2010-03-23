--TEST--
SPL: SplObjectStorage: recursive var_dump
--FILE--
<?php
$o = new SplObjectStorage();

$o[new StdClass] = $o;

var_dump($o);
--EXPECTF--
object(SplObjectStorage)#1 (1) {
  ["storage":"SplObjectStorage":private]=>
  array(1) {
    ["%s"]=>
    array(2) {
      ["obj"]=>
      object(stdClass)#2 (0) {
      }
      ["inf"]=>
      object(SplObjectStorage)#1 (1) {
        ["storage":"SplObjectStorage":private]=>
        array(1) {
          ["%s"]=>
          array(2) {
            ["obj"]=>
            object(stdClass)#2 (0) {
            }
            ["inf"]=>
            *RECURSION*
          }
        }
      }
    }
  }
}
