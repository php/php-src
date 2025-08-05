--TEST--
PFA variation: called scope
--FILE--
<?php
class Foo {
    public function method($a) {
        printf("%s::%s\n", get_called_class(), __FUNCTION__);
    }
}

$foo = new Foo;

$bar = $foo->method(new stdClass, ...);

$bar();
?>
--EXPECT--
Foo::method
