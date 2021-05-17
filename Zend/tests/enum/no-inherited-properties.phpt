--TEST--
Enum disallows inherited instance properties
--FILE--
<?php

trait T {
    public $bar;
}

enum Foo {
    use T;
}

?>
--EXPECTF--
Fatal error: Enum "Foo" may not include instance properties in %s on line 7
