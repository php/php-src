--TEST--
shm_detach() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
?>
--FILE--
<?php

try {
    shm_detach();
} catch (ArgumentCountError $error) {
    echo $error->getMessage() . PHP_EOL;
}
$key = 456;
try {
    var_dump(shm_detach($key,1));
} catch (TypeError $error) {
    echo $error->getMessage() . PHP_EOL;
}

$s = shm_attach($key);
var_dump($s);
var_dump(shm_detach($s));

try {
    var_dump(shm_detach($s));
} catch (InvalidArgumentException $exception) {
    echo $exception->getMessage() . PHP_EOL;
}

try {
    shm_remove($s);
} catch (InvalidArgumentException $exception) {
    echo $exception->getMessage() . PHP_EOL;
}

echo '=====' . PHP_EOL;
try {
    shm_put_var($s, 1, "value!");
} catch (InvalidArgumentException $exception) {
    echo $exception->getMessage() . PHP_EOL;
}
try {
    shm_has_var($s, 1);
} catch (InvalidArgumentException $exception) {
    echo $exception->getMessage() . PHP_EOL;
}
try {
    shm_get_var($s, 1);
} catch(InvalidArgumentException $excception) {
    echo $excception->getMessage() . PHP_EOL;
}
echo "Done\n";
?>
--CLEAN--
<?php

$s = shm_attach(456);
shm_remove($s);

?>
--EXPECT--	
shm_detach() expects exactly 1 parameter, 0 given
Argument 1 passed to shm_detach() must be an instance of SharedMemoryBlock, int given
object(SharedMemoryBlock)#1 (0) {
}
NULL
Invalid SharedMemoryBlock object internal state
Invalid SharedMemoryBlock object internal state
=====
Invalid SharedMemoryBlock object internal state
Invalid SharedMemoryBlock object internal state
Invalid SharedMemoryBlock object internal state
Done