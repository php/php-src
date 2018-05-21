--TEST--
shm_get_var() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
?>
--FILE--
<?php

$s = shm_attach(456, 1024);

shm_put_var($s, -1, "test string");
shm_put_var($s, 0, new stdclass);
shm_put_var($s, 1, array(1,2,3));
shm_put_var($s, 2, false);
shm_put_var($s, 3, null);

try {
    shm_get_var();
} catch (TypeError $error) {
    echo $error->getMessage() . PHP_EOL;
}
try {
    shm_get_var(-1, -1);
} catch (TypeError $error) {
    echo $error->getMessage() . PHP_EOL;
}
try {
    shm_get_var($s, 1000);
} catch (OutOfBoundsException $exception) {
    echo $exception->getMessage() . PHP_EOL;
}
try {
    shm_get_var($s, -10000);
} catch (OutOfBoundsException $exception) {
    echo $exception->getMessage() . PHP_EOL;
}

try {
    shm_get_var($s, array());
} catch (TypeError $error) {
    echo $error->getMessage() . PHP_EOL;
}
var_dump(shm_get_var($s, -1));
var_dump(shm_get_var($s, 0));
var_dump(shm_get_var($s, 1));
var_dump(shm_get_var($s, 2));
var_dump(shm_get_var($s, 3));

shm_put_var($s, 3, "test");
shm_put_var($s, 3, 1);
shm_put_var($s, 3, null);

var_dump(shm_get_var($s, 3));
shm_remove($s);

echo "Done\n";
?>
--CLEAN--
<?php

$s = shm_attach(456);
shm_remove($s);

?>
--EXPECT--	
shm_get_var() expects exactly 2 parameters, 0 given
Argument 1 passed to shm_get_var() must be an instance of SharedMemoryBlock, int given
Variable key 1000 doesn't exist
Variable key -10000 doesn't exist
Argument 2 passed to shm_get_var() must be of the type int, array given
string(11) "test string"
object(stdClass)#3 (0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
bool(false)
NULL
NULL
Done