--TEST--
Bug GH-9285: PHP 8.2 readonly classes allow inheriting mutable properties from traits - error
--FILE--
<?php

trait T {
    public $prop;
}

readonly class C {
    use T;
}

?>
--EXPECTF--
Fatal error: Readonly class C cannot use trait with a non-readonly property T::$prop in %s on line %d
