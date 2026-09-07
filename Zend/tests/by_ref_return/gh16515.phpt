--TEST--
GH-16515: Incorrect propagation of ZEND_ACC_RETURN_REFERENCE for call trampoline
--FILE--
<?php

namespace Foo;

class Foo {
    public function &__call($method, $args) {}
}

call_user_func((new Foo)->bar(...));

?>
--EXPECTF--
Notice: Only variable references should be returned by reference in %s on line %d
