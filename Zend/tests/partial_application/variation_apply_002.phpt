--TEST--
Partial application variation type
--FILE--
<?php
class Foo {
    public function __construct() {
        printf("%s\n", __METHOD__);
    }
}

$foo = new Foo(...);

$bar = $foo(...);

$bar();
?>
--EXPECT--
Foo::__construct
