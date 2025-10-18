--TEST--
#[NotSerializable] inheritance behavior
--FILE--
<?php

#[NotSerializable]
class Foo {
    public int $x = 42;
}

class Bar extends Foo {}

try {
    $s = serialize(new Bar());
    echo "Should not reach here\n";
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $data = 'O:3:"Bar":1:{s:1:"x";i:42;}';
    unserialize($data);
    echo "Should not reach here\n";
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Serialization of 'Bar' is not allowed
Unserialization of 'Bar' is not allowed
