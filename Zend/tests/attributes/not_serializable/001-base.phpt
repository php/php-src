--TEST--
#[NotSerializable] basic behavior
--FILE--
<?php

#[NotSerializable]
class Foo {
    public int $x = 42;
}

try {
    $s = serialize(new Foo());
    echo "Should not reach here\n";
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $data = 'O:3:"Foo":1:{s:1:"x";i:42;}';
    unserialize($data);
    echo "Should not reach here\n";
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Serialization of 'Foo' is not allowed
Unserialization of 'Foo' is not allowed
