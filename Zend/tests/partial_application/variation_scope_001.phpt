--TEST--
Partial application variation called scope
--FILE--
<?php
class Foo {
    public function method() {
        printf("%s::%s\n", get_called_class(), __FUNCTION__);
    }
}

$foo = new Foo;

$bar = $foo->method(...);

$bar();
?>
--EXPECT--
Foo::method
