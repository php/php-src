--TEST--
shm_detach() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
?>
--FILE--
<?php

$key = 123;

var_dump(shm_detach());
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
} catch (TypeError $error) {
    echo $error->getMessage() . PHP_EOL;
}
shm_remove($s);
var_dump($s);
var_dump(shm_detach($s));
var_dump($s);
echo '=====' . PHP_EOL;
shm_put_var($s, 1, "value!");
var_dump(shm_get_var($s, 1));
echo "Done\n";
?>
--CLEAN--
<?php

$s = shm_attach(123);
shm_remove($s);

?>
--EXPECTF--	
Warning: shm_detach() expects exactly 1 parameter, 0 given in %s003.php on line 5
NULL
Argument 1 passed to shm_detach() must be an instance of SharedMemoryBlock, int given
object(SharedMemoryBlock)#2 (0) {
}

Warning: shm_detach() expects parameter 1 to be SharedMemoryBlock, object given in %s003.php on line 14
NULL

Warning: shm_detach() expects parameter 1 to be SharedMemoryBlock, object given in %s003.php on line 16
NULL

Warning: shm_remove() expects parameter 1 to be SharedMemoryBlock, object given in %s003.php on line 20
object(SharedMemoryBlock)#2 (0) {
}

Warning: shm_detach() expects parameter 1 to be SharedMemoryBlock, object given in %s003.php on line 22
NULL
Done