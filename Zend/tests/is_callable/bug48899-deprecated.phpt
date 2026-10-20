--TEST--
Bug #48899 (is_callable returns true even if method does not exist in parent class) [original test with deprecated syntax]
--FILE--
<?php

class ParentClass { }

class ChildClass extends ParentClass {
    public function testIsCallable() {
        var_dump(is_callable(array($this, 'parent::testIsCallable')));
    }
    public function testIsCallable2() {
        var_dump(is_callable(array($this, 'static::testIsCallable2')));
    }
}

$child = new ChildClass();
$child->testIsCallable();
$child->testIsCallable2();

?>
--EXPECTF--
Deprecated: Callables of the form ["ChildClass", "parent::testIsCallable"] are deprecated in %s on line %d
bool(false)

Deprecated: Callables of the form ["ChildClass", "static::testIsCallable2"] are deprecated in %s on line %d
bool(true)
