--TEST--
Bug #60145 (Usage of trait's use statement inside interfaces not properly checked.)
--FILE--
<?php

trait foo {

}

interface MyInterface {
    use foo;

    public function b();

}
?>
--EXPECTF--
Fatal error: Cannot use traits inside of interfaces. foo is used in MyInterface in %s on line %d
