--TEST--
GH-17991 (Assertion failure dom_attr_value_write)
--EXTENSIONS--
dom
--FILE--
<?php
$attr = new DOMAttr("r", "iL");
class Box {
    public ?Test $value;
}
class Test {
}
function test($box) {
    var_dump($box->value = new Test);
}
$box = new Box();
test($box);
test($attr);
?>
--EXPECTF--
object(Test)#%d (0) {
}

Fatal error: Uncaught TypeError: Cannot assign Test to property DOMAttr::$value of type string in %s:%d
Stack trace:
#0 %s(%d): test(Object(DOMAttr))
#1 {main}
  thrown in %s on line %d
