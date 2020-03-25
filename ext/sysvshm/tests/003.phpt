--TEST--
shm_detach() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
if (!function_exists('ftok')){ print 'skip'; }
?>
--FILE--
<?php

$key = ftok(__DIR__."/003.phpt", 'q');

$s = shm_attach($key);

var_dump(shm_detach($s));
try {
    var_dump(shm_detach($s));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    shm_remove($s);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
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
shm_detach(): supplied resource is not a valid sysvshm resource
shm_remove(): supplied resource is not a valid sysvshm resource
Done
