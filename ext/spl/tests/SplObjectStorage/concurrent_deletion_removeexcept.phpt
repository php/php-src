--TEST--
SplObjectStorage: Mutation during getHash is prohibited during removeAllExcept
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
    $storage->removeAllExcept($evil);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump(count($evil), count($storage));

?>
--EXPECT--
Error: Modification of SplObjectStorage during getHash() is prohibited
int(0)
int(1)
