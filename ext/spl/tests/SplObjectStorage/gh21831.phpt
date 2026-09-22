--TEST--
GH-21831: SplObjectStorage::getHash() cannot mutate storage during removeAllExcept()
--FILE--
<?php

class FilterStorage extends SplObjectStorage {
    public ?SplObjectStorage $other = null;

    public function getHash($obj): string {
        if ($this->other) {
            $this->other->offsetUnset($obj);
            $this->other = null;
        }

        return 'x';
    }
}

$storage = new SplObjectStorage();
$storage[new stdClass()] = null;

$filter = new FilterStorage();
$filter->other = $storage;

try {
    $storage->removeAllExcept($filter);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump(count($storage));

?>
--EXPECT--
Error: Modification of SplObjectStorage during getHash() is prohibited
int(1)
