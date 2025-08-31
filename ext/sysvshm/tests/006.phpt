--TEST--
shm_remove_var() tests
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

try {
    shm_put_var($s, 1, "test string");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(shm_remove_var($s, -10));

var_dump(shm_get_var($s, 1));

var_dump(shm_remove_var($s, 1));
var_dump(shm_get_var($s, 1));

var_dump(shm_remove_var($s, 1));
var_dump(shm_get_var($s, 1));

shm_remove($s);
echo "Done\n";
?>
--EXPECTF--
Warning: shm_remove_var(): Variable key -10 doesn't exist in %s006.php on line %d
bool(false)
string(11) "test string"
bool(true)

Warning: shm_get_var(): Variable key 1 doesn't exist in %s006.php on line %d
bool(false)

Warning: shm_remove_var(): Variable key 1 doesn't exist in %s006.php on line %d
bool(false)

Warning: shm_get_var(): Variable key 1 doesn't exist in %s006.php on line %d
bool(false)
Done
