--TEST--
GH-16479: Use-after-free in SplObjectStorage::setInfo()
--FILE--
<?php

class C {
    function __destruct() {
        global $store;
        $store->removeAll($store);
    }
}

$o = new stdClass;
$store = new SplObjectStorage;
$store[$o] = new C;
$store->setInfo(1);
var_dump($store);

?>
--EXPECT--
object(SplObjectStorage)#2 (1) {
  ["storage":"SplObjectStorage":private]=>
  array(0) {
  }
}
