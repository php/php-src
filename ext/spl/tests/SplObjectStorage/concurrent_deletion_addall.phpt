--TEST--
SplObjectStorage: Mutation during getHash is prohibited during addAll
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
try {
    $evil->addAll($storage);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump(count($evil), count($storage));

?>
--EXPECT--
Modification of SplObjectStorage during getHash() is prohibited
int(0)
int(1)
