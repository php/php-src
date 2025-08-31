--TEST--
shm_remove() tests
--EXTENSIONS--
sysvshm
--SKIPIF--
<?php

if (!function_exists('ftok')){ print 'skip'; }
?>
--FILE--
<?php

$key = ftok(__FILE__, 't');
$s = shm_attach($key, 1024);

var_dump(shm_remove($s));

shm_detach($s);
try {
    shm_remove($s);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECT--
bool(true)
Shared memory block has already been destroyed
Done
