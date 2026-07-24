--TEST--
Readonly class can extend a readonly class
--FILE--
<?php

readonly class Foo
{
    public int $prop = 1;
}

readonly class Bar extends Foo
{
    public int $prop = 2;
}

readonly class Baz extends Foo {}

var_dump(new Foo()->prop);
var_dump(new Bar()->prop);
var_dump(new Baz()->prop);

?>
--EXPECT--
int(1)
int(2)
int(1)
