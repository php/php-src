--TEST--
shm_detach() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
?>
--FILE--
<?php

$key = 456;

$s = shm_attach($key);
var_dump($s);
shm_detach($s);
try {
    shm_detach($s);
} catch (InvalidArgumentException $exception) {
    echo $exception->getMessage().PHP_EOL;
}

var_dump($s);
echo "Done\n";
?>
--CLEAN--
<?php

$s = shm_attach(456);
shm_remove($s);

?>
--EXPECT--	
object(SharedMemoryBlock)#1 (0) {
}
Invalid SharedMemoryBlock object internal state
object(SharedMemoryBlock)#1 (0) {
}
Done