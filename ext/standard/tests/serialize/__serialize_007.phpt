--TEST--
No packed -> mixed reallocation while populating __unserialize() array
--FILE--
<?php

$payload = 'O:13:"ArrayIterator":2:{i:0;i:0;s:1:"x";R:2;}';
try {
    var_dump(unserialize($payload));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
UnexpectedValueException: Incomplete or ill-typed serialization data
