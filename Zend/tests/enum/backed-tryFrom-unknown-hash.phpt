--TEST--
BackedEnum::tryFrom() unknown hash
--FILE--
<?php

enum Foo: string {
    case Bar = 'B';
}

$s = 'A';
$s++;

var_dump(Foo::tryFrom($s));

?>
--EXPECT--
enum(Foo::Bar)
