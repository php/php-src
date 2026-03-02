--TEST--
__serialize() exception in shm_put_var()
--EXTENSIONS--
sysvshm
--FILE--
<?php

$key = ftok(__FILE__, 't');
$s = shm_attach($key, 1024);

class Test {
    public function __serialize() {
        throw new Error("no");
    }
}

try {
    shm_put_var($s, 1, new Test);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

shm_remove($s);

?>
--EXPECT--
no
