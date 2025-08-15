--TEST--
BackedEnum::from() unknown hash
--FILE--
<?php

enum Foo: string {
    case Bar = 'B';
}

$s = 'A';
$s++;

var_dump(Foo::from($s));

?>
--EXPECTF--
Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
enum(Foo::Bar)
