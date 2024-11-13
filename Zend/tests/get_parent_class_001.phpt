--TEST--
Testing get_parent_class()
--FILE--
<?php

interface ITest {
    function foo();
}

abstract class bar implements ITest {
    public function foo() {
        var_dump(get_parent_class());
    }
}

class foo extends bar {
    public function __construct() {
        var_dump(get_parent_class());
    }
}

$a = new foo;
$a->foo();

?>
--EXPECTF--
Deprecated: Calling get_parent_class() without arguments is deprecated in %s on line %d
string(3) "bar"

Deprecated: Calling get_parent_class() without arguments is deprecated in %s on line %d
bool(false)
