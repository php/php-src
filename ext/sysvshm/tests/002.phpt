--TEST--
shm_attach() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
if (!function_exists('ftok')){ print 'skip'; }
?>
--FILE--
<?php

$key = ftok(__FILE__, 't');

try {
    shm_attach(-1, 0);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    shm_attach(0, -1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    shm_attach(123, -1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    shm_attach($key, -1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    shm_attach($key, 0);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump($s = shm_attach($key, 1024));
shm_remove($s);
var_dump($s = shm_attach($key, 1024));
shm_remove($s);
var_dump($s = shm_attach($key, 1024, 0666));
shm_remove($s);

var_dump($s = shm_attach($key, 1024));
shm_remove($s);
var_dump($s = shm_attach($key));
shm_remove($s);

echo "Done\n";
?>
--EXPECTF--
shm_attach(): Argument #2 ($size) must be greater than 0
shm_attach(): Argument #2 ($size) must be greater than 0
shm_attach(): Argument #2 ($size) must be greater than 0
shm_attach(): Argument #2 ($size) must be greater than 0
shm_attach(): Argument #2 ($size) must be greater than 0
object(SysvSharedMemory)#%d (0) {
}
object(SysvSharedMemory)#%d (0) {
}
object(SysvSharedMemory)#%d (0) {
}
object(SysvSharedMemory)#%d (0) {
}
object(SysvSharedMemory)#%d (0) {
}
Done
