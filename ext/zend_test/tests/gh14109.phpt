--TEST--
GH-14109: User class extending internal class with attributes
--EXTENSIONS--
zend_test
--FILE--
<?php
class Test extends ZendAttributeTest {}
foreach ((new ReflectionClassConstant(Test::class, 'TEST_CONST'))->getAttributes() as $attribute) {
    var_dump($attribute->newInstance());
}
?>
--EXPECTF--
object(ZendTestRepeatableAttribute)#%d (0) {
}
object(ZendTestRepeatableAttribute)#%d (0) {
}
