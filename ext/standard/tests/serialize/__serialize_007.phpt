--TEST--
No packed -> mixed reallocation while populating __unserialize() array
--FILE--
<?php

$payload = 'O:13:"ArrayIterator":2:{i:0;i:0;s:1:"x";R:2;}';
try {
    var_dump(unserialize($payload));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Incomplete or ill-typed serialization data
