--TEST--
shm_remove() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
?>
--FILE--
<?php

$s = shm_attach(456, 1024);

try {
    shm_remove();
} catch (ArgumentCountError $error) {
    echo $error->getMessage() . PHP_EOL;
}
try {
    shm_remove(-1);
} catch (TypeError $error) {
    echo $error->getMessage() . PHP_EOL;
}
try {
    shm_remove(0);
} catch (TypeError $error) {
    echo $error->getMessage() . PHP_EOL;
}
try {
    shm_remove("");
} catch (TypeError $error) {
    echo $error->getMessage() . PHP_EOL;
}

var_dump(shm_remove($s));

try {
    shm_detach($s);
} catch (InvalidArgumentException $exception) {
    echo $exception->getMessage() . PHP_EOL;
}
try {
    shm_remove($s);
} catch (InvalidArgumentException $exception) {
    echo $exception->getMessage() . PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--	
shm_remove() expects exactly 1 parameter, 0 given
Argument 1 passed to shm_remove() must be an instance of SharedMemoryBlock, int given
Argument 1 passed to shm_remove() must be an instance of SharedMemoryBlock, int given
Argument 1 passed to shm_remove() must be an instance of SharedMemoryBlock, string given
NULL
Invalid SharedMemoryBlock object internal state
Invalid SharedMemoryBlock object internal state
Done