--TEST--
shm_attach() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
if (!function_exists('ftok')){ print 'skip'; }
?>
--FILE--
<?php

try {
    shm_attach();
} catch (ArgumentCountError $error) {
    echo $error->getMessage() . PHP_EOL;
}

try {
    shm_attach(1,2,3,4);
} catch (ArgumentCountError $error) {
    echo $error->getMessage() . PHP_EOL;
}

try {
    shm_attach(-1, 0);
} catch (InvalidArgumentException $exception) {
    echo $exception->getMessage() . PHP_EOL;
}

try {
    shm_attach(0, -1);
} catch (InvalidArgumentException $exception) {
    echo $exception->getMessage() . PHP_EOL;
}

try {
    shm_attach(123, -1);
} catch (InvalidArgumentException $exception) {
    echo $exception->getMessage() . PHP_EOL;
}
echo '====' . PHP_EOL;

var_dump($s = shm_attach(123, 1024));
shm_remove($s);

var_dump($s = shm_attach(123, 1024, 0666));
shm_remove($s);

var_dump($s = shm_attach(123, 1024));
shm_remove($s);

var_dump($s = shm_attach(123));
shm_remove($s);

echo "Done\n";
?>
--EXPECTF--	
shm_attach() expects at least 1 parameter, 0 given
shm_attach() expects at most 3 parameters, 4 given
Segment size 0 must be greater than zero
Segment size -1 must be greater than zero
Segment size -1 must be greater than zero
====
object(SharedMemoryBlock)#5 (0) {
}
object(SharedMemoryBlock)#7 (0) {
}
object(SharedMemoryBlock)#5 (0) {
}
object(SharedMemoryBlock)#7 (0) {
}
Done