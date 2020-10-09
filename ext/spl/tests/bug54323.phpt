--TEST--
Bug #54323 (Accessing unset()'ed ArrayObject's property causes crash)
--FILE--
<?php
class C {
        public $prop = 'C::prop.orig';
}
class MyArrayObject extends ArrayObject {
}
$c = new C;
$ao = new MyArrayObject($c);
testAccess($c, $ao);
function testAccess($c, $ao) {
        foreach ($ao as $key=>$value) {
        }
        unset($ao['prop']);
        var_dump($c->prop, $ao['prop']);
}
?>
--EXPECTF--
Warning: Undefined property: C::$prop in %s on line %d

Warning: Undefined array key "prop" in %s on line %d
NULL
NULL
