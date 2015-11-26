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
--EXPECTF--
Notice: Undefined property: C::$prop in %sbug54323.php on line 14

Notice: Undefined index: prop in %sbug54323.php on line 14
NULL
NULL
