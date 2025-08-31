--TEST--
Enum is final
--FILE--
<?php

enum Foo {}

$final = new ReflectionClass(Foo::class)->isFinal();
var_dump($final);

?>
--EXPECT--
bool(true)
