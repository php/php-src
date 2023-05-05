--TEST--
Ownership of constant expression inhereted from immutable class should be transfered to class
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
