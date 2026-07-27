--TEST--
SplObjectStorage: Concurrent deletion during iteration
--CREDITS--
cnitlrt
--FILE--
<?php
class EvilStorage extends SplObjectStorage {
    public function getHash($obj): string {
        global $victim;
        static $counter = 0;
        if ($counter++ < 1024*2) {
            // Re-entrant mutation during removeAllExcept iteration
            for ($i = 0; $i < 5; $i++) {
                $victim[new stdClass()] = null;
            }
        }
        return spl_object_hash($obj);
    }
}

$victim = new SplObjectStorage();
$other = new EvilStorage();

for ($i = 0; $i < 1024; $i++) {
    $o = new stdClass();
    $victim[$o] = null;
    $other[$o] = null;
}

var_dump($victim->removeAllExcept($other));

unset($victim, $other);
$victim = new SplObjectStorage();
$other = new EvilStorage();

for ($i = 0; $i < 1024; $i++) {
    $o = new stdClass();
    $victim[$o] = null;
    $other[$o] = null;
}

var_dump($other->addAll($victim));
?>
--EXPECTF--
int(%d)
int(1024)
