--TEST--
oss-fuzz #57821: Unevaluated rhs of class constant fetch in constant expression
--FILE--
<?php
class Foo {
    const Foo = 'foo';
}
const C = Foo::{Foo::class};
var_dump(C);
?>
--EXPECT--
string(3) "foo"
