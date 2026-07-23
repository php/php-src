--TEST--
SplObjectStorage: Mutation during getHash is prohibited
--CREDITS--
cnitlrt
--FILE--
<?php
class EvilStorage extends SplObjectStorage {
    public bool $mutate = false;

    public function getHash($obj): string {
        global $victim;
        if ($this->mutate) {
            $victim[new stdClass()] = null;
        }
        return spl_object_hash($obj);
    }
}

function populate(SplObjectStorage $victim, SplObjectStorage $other): void {
    for ($i = 0; $i < 1024; $i++) {
        $o = new stdClass();
        $victim[$o] = null;
        $other[$o] = null;
    }
}

$victim = new SplObjectStorage();
$other = new EvilStorage();

populate($victim, $other);
$other->mutate = true;

try {
    $victim->removeAllExcept($other);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump(count($victim), count($other));

unset($victim, $other);
$victim = new SplObjectStorage();
$other = new EvilStorage();

populate($victim, $other);
$other->mutate = true;

try {
    $other->addAll($victim);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump(count($victim), count($other));
?>
--EXPECT--
Modification of SplObjectStorage during getHash() is prohibited
int(1024)
int(1024)
Modification of SplObjectStorage during getHash() is prohibited
int(1024)
int(1024)
