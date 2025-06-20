--TEST--
ReflectionClass::__constructor() - bad arguments
--FILE--
<?php

try {
    var_dump(new ReflectionClass(1));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(new ReflectionClass("X"));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
Class "1" does not exist
Class "X" does not exist
