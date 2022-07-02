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
--EXPECT--
enum(Foo::Bar)
