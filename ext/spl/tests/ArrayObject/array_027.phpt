--TEST--
SPL: ArrayObject revursive var_dump
--FILE--
<?php
class AO extends ArrayObject {
}
$o = new AO();
$o['plop'] = $o;

var_dump($o);
?>
--EXPECTF--
object(AO)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    ["plop"]=>
    *RECURSION*
  }
}
