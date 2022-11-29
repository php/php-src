--TEST--
Enum traits no __construct
--FILE--
<?php

trait Foo {
    public function __construct() {
        echo "Evil code\n";
    }
}

enum Bar {
    use Foo;
    case Baz;
}

var_dump(Bar::Baz);

?>
--EXPECTF--
Fatal error: Enum Bar cannot include magic method __construct in %s on line %d
