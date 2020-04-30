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
var_dump(shm_detach($s));
shm_remove($s);

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
bool(true)
Done
