--TEST--
#[NotSerializable] enum behavior
--FILE--
<?php

#[NotSerializable]
enum Foo {
    case BAR;
}

try {
    $s = serialize(Foo::BAR);
    echo "Should not reach here\n";
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $data = 'E:7:"Foo:BAR";';
    unserialize($data);
    echo "Should not reach here\n";
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Serialization of 'Foo' is not allowed
Unserialization of 'Foo' is not allowed
