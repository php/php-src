--TEST--
shm_detach() tests
--EXTENSIONS--
sysvshm
--SKIPIF--
<?php

if (!function_exists('ftok')){ print 'skip'; }
?>
--FILE--
<?php

$key = ftok(__DIR__."/003.phpt", 'q');

$s = shm_attach($key);

var_dump(shm_detach($s));
try {
    shm_detach($s);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    shm_remove($s);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--CLEAN--
<?php

$key = ftok(__DIR__."/003.phpt", 'q');
$s = shm_attach($key);
shm_remove($s);

?>
--EXPECT--
bool(true)
Shared memory block has already been destroyed
Shared memory block has already been destroyed
Done
