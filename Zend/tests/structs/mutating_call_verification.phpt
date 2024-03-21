--TEST--
Mutating method calls on structs
--FILE--
<?php

struct D {
    public function nonMutating() {
        echo __FUNCTION__, "\n";
    }

    public mutating function mutating() {
        echo __FUNCTION__, "\n";
    }
}

$d = new D();

$d->nonMutating();
$d->mutating!();

try {
    $d->nonMutating!();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    $d->mutating();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
nonMutating
mutating
Non-mutating method must not be called with $object->func!() syntax
Mutating method must be called with $object->func!() syntax
