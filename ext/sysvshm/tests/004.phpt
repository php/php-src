--TEST--
shm_put_var() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
?>
--FILE--
<?php

$s = shm_attach(456, 1024);

try {
    shm_put_var();
} catch (ArgumentCountError $error) {
    echo $error->getMessage();
}
try {
    shm_put_var(-1, -1, -1);
} catch (Throwable $error) {
    echo get_class($error) . PHP_EOL;
}
try {
    shm_put_var(-1, 10, "qwerty");
} catch (Throwable $error) {
    echo get_class($error) . PHP_EOL;
}
var_dump(shm_put_var($s, -1, "qwerty"));
var_dump(shm_put_var($s, 10, "qwerty"));
var_dump(shm_put_var($s, 10, "qwerty"));
try {
    $string = str_repeat("test", 512);
    var_dump(shm_put_var($s, 11, $string));
} catch (Throwable $error) {
    echo get_class($error) . PHP_EOL;
}

shm_remove($s);

echo "Done\n";
?>
--CLEAN--
<?php

$s = shm_attach(456);
shm_remove($s);

?>
--EXPECT--	
shm_put_var() expects exactly 3 parameters, 0 givenTypeError
TypeError
NULL
NULL
NULL
RuntimeException
Done