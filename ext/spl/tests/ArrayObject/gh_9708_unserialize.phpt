--TEST--
GH-9708: object_properties_load() bypasses typed property checks
--FILE--
<?php

class Foo extends ArrayObject {
    public int $a = 5;
    public string $b = "10";
}

try {
    // a = "10", b = 5
    unserialize('O:3:"Foo":4:{i:0;i:0;i:1;a:0:{}i:2;a:2:{s:1:"a";s:2:"10";s:1:"b";i:5;}i:3;N;}');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}


?>
--EXPECT--
TypeError: Cannot assign string to property Foo::$a of type int
