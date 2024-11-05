--TEST--
GH-16591 (Assertion error in shm_put_var)
--EXTENSIONS--
sysvshm
--FILE--
<?php

class C {
    function __serialize(): array {
        global $mem;
        shm_detach($mem);
        return ['a' => 'b'];
    }
}

$mem = shm_attach(1);
try {
    shm_put_var($mem, 1, new C);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Shared memory block has been destroyed by the serialization function
