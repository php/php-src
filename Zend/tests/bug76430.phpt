--TEST--
Bug #76430: __METHOD__ inconsistent outside of method
--FILE--
<?php

class Foo {
    const X = __METHOD__;
}
function foo() {
    class Bar {
        const X = __METHOD__;
    }
}

foo();
var_dump(Foo::X);
var_dump(Bar::X);

?>
--EXPECT--
string(0) ""
string(0) ""
