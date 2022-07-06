--TEST--
Bug #53915 - ReflectionClass::getConstant(s) emits fatal error on selfreferencing constants
--FILE--
<?php
Class Foo
{
    const A = 1;
    const B = self::A;
}

$rc = new ReflectionClass('Foo');
print_r($rc->getConstants());

Class Foo2
{
        const A = 1;
        const B = self::A;
}

$rc = new ReflectionClass('Foo2');
print_r($rc->getConstant('B'));
?>
--EXPECT--
Array
(
    [A] => 1
    [B] => 1
)
1
