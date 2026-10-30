--TEST--
SplObjectStorage: Concurrent deletion during addAll
--CREDITS--
cnitlrt
--FILE--
<?php

class EvilStorage extends SplObjectStorage {
    public function getHash($obj): string {
        global $storage;
        unset($storage[$obj]);
        return spl_object_hash($obj);
    }
}

$storage = new SplObjectStorage();
$storage[new stdClass] = 'foo';

$evil = new EvilStorage();
$evil->addAll($storage);

var_dump($evil, $storage);

?>
--EXPECTF--
object(EvilStorage)#%d (1) {
  ["storage":"SplObjectStorage":private]=>
  array(1) {
    [0]=>
    array(2) {
      ["obj"]=>
      object(stdClass)#%d (0) {
      }
      ["inf"]=>
      string(3) "foo"
    }
  }
}
object(SplObjectStorage)#%d (1) {
  ["storage":"SplObjectStorage":private]=>
  array(0) {
  }
}
