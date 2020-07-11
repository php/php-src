--TEST--
Unserializing an abstract class should fail
--FILE--
<?php

$payload = 'O:23:"RecursiveFilterIterator":0:{}';
try {
    var_dump(unserialize($payload));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot instantiate abstract class RecursiveFilterIterator
