--TEST--
shm_remove_var() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
?>
--FILE--
<?php

$s = shm_attach(456, 1024);

shm_put_var($s, 1, "test string");

try {
    shm_remove_var();
} catch (ArgumentCountError $error) {
    echo $error->getMessage() . PHP_EOL;
}
try {
    shm_remove_var(-1, -1);
} catch (TypeError $error) {
    echo $error->getMessage() . PHP_EOL;
}
try {
    shm_remove_var($s, -10);
} catch (OutOfBoundsException $error) {
    echo $error->getMessage() . PHP_EOL;
}
var_dump(shm_get_var($s, 1));
shm_remove_var($s, 1);
try {
    shm_get_var($s, 1);
} catch (OutOfBoundsException $error) {
    echo $error->getMessage() . PHP_EOL;
}

try {
    shm_remove_var($s, 1);
} catch (OutOfBoundsException $error) {
    echo $error->getMessage() . PHP_EOL;
}
try {
    shm_get_var($s, 1);
} catch (OutOfBoundsException $error) {
    echo $error->getMessage() . PHP_EOL;
}

shm_remove($s);
echo "Done\n";
?>
--EXPECT--	
shm_remove_var() expects exactly 2 parameters, 0 given
Argument 1 passed to shm_remove_var() must be an instance of SharedMemoryBlock, int given
Variable key -10 doesn't exist
string(11) "test string"
Variable key 1 doesn't exist
Variable key 1 doesn't exist
Variable key 1 doesn't exist
Done