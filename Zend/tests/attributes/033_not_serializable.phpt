--TEST--
#[NotSerializable]
--FILE--
<?php

#[NotSerializable]
class C {}

class D extends C {}

try {
    serialize(new C());
} catch (Throwable $ex) {
    echo $ex->getMessage() . "\n";
}

try {
    var_dump(unserialize('O:1:"C":0:{}'));
} catch (Throwable $ex) {
    echo $ex->getMessage() . "\n";
}

try {
    serialize(new D());
} catch (Throwable $ex) {
    echo $ex->getMessage() . "\n";
}

--EXPECTF--
Serialization of 'C' is not allowed
Unserialization of 'C' is not allowed
Serialization of 'D' is not allowed
