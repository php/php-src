--TEST--
SPL: SplObjectStorage: recursive var_dump
--FILE--
<?php
$o = new SplObjectStorage();

$o[new StdClass] = $o;

var_dump($o);
--EXPECTF--
object(SplObjectStorage)#%d (1) {
  ["storage":"SplObjectStorage":private]=>
  array(1) {
    [0]=>
    array(2) {
      ["obj"]=>
      object(stdClass)#%d (0) {
      }
      ["inf"]=>
      *RECURSION*
    }
  }
}
