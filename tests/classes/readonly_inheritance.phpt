--TEST--
Inheritance of readonly properties
--FILE--
<?php

// OK
class Foo {
    public readonly $x;
}

class Bar extends Foo {
    public readonly $x;
}

// OK
class Foo2 {
    public readonly $x;
}

class Bar2 extends Foo2 {
    public $x;
}

// Bang!
class Foo3 {
    public $x;
}

class Bar3 extends Foo3 {
    public readonly $x;
}
--EXPECTF--
Fatal error: Bar3::$x cannot be readonly as parent in class Foo3 is not readonly in %s on line %d
