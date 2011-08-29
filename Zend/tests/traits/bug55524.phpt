--TEST--
Bug #55524 Traits should not be able to extend a class
--FILE--
<?php

class Base {}

trait Foo extends Base {
    function bar() {}
}

echo 'DONE';
?>
--EXPECTF--
Fatal error: A trait (Foo) cannot extend a class in %s on line %d
