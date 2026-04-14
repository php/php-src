--TEST--
SplObjectStorage: Concurrent deletion during removeAllExcept
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
$storage->removeAllExcept($evil);

var_dump($evil, $storage);

?>
--EXPECTF--
object(EvilStorage)#%d (1) {
  ["storage":"SplObjectStorage":private]=>
  array(0) {
  }
}
object(SplObjectStorage)#%d (1) {
  ["storage":"SplObjectStorage":private]=>
  array(0) {
  }
}
