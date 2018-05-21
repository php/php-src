--TEST--
SharedMemoryBlock::__destruct() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
?>
--FILE--
<?php

$s = new SharedMemoryBlock(456);
var_dump($s);
unset($s);
var_dump($s);
$s = new SharedMemoryBlock(456);
var_dump($s);
unset($s);
var_dump($s);
echo "Done\n";
?>
--CLEAN--
<?php

$s = shm_attach(456);
shm_remove($s);

?>
--EXPECTF--	
object(SharedMemoryBlock)#1 (0) {
}

Notice: Undefined variable: s in %s on line %d
NULL
object(SharedMemoryBlock)#1 (0) {
}

Notice: Undefined variable: s in %s on line %d
NULL
Done