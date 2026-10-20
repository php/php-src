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

$key = ftok(__FILE__, 't');
$mem = shm_attach($key);
$cleanup = shm_attach($key);
try {
    shm_put_var($mem, 1, new C);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
} finally {
    shm_remove($cleanup);
}

?>
--EXPECT--
Error: Shared memory block has been destroyed by the serialization function
