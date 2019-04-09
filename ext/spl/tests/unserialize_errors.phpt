--TEST--
Errors from __unserialize() with invalid data
--FILE--
<?php

try {
    unserialize('O:11:"ArrayObject":0:{}');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    unserialize('O:11:"ArrayObject":3:{i:0;b:1;i:1;a:0:{}i:2;a:0:{}}');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    unserialize('O:11:"ArrayObject":3:{i:0;i:0;i:1;a:0:{}i:2;i:0;}');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    unserialize('O:11:"ArrayObject":3:{i:0;i:0;i:1;i:0;i:2;a:0:{}}');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    unserialize('O:11:"ArrayObject":3:{i:0;i:0;i:1;a:0:{}i:2;i:0}');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Incomplete or ill-typed serialization data
Incomplete or ill-typed serialization data
Incomplete or ill-typed serialization data
Passed variable is not an array or object

Notice: unserialize(): Error at offset 44 of 48 bytes in %s on line %d
