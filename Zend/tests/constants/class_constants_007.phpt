--TEST--
Ownership of constant expression inherited from immutable class should be tranferred to class
--FILE--
<?php
class A {
    const X = ' ' . self::Y;
    const Y = ' ';
}
eval('class B extends A{}');
var_dump(B::X);
?>
--EXPECT--
string(2) "  "
