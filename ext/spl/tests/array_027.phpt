--TEST--
SPL: ArrayObject revursive var_dump
--FILE--
<?php
class AO extends ArrayObject {
}
$o = new AO();
$o['plop'] = $o;

var_dump($o);
--EXPECTF--
object(AO)#1 (1) {
  [u"storage":u"ArrayObject":private]=>
  array(1) {
    [u"plop"]=>
    object(AO)#1 (1) {
      [u"storage":u"ArrayObject":private]=>
      array(1) {
        [u"plop"]=>
        *RECURSION*
      }
    }
  }
}
