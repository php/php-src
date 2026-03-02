--TEST--
Bug #77673 ReflectionClass::getDefaultProperties returns spooky array
--FILE--
<?php
class A {
    public B $c;
}

$class = new ReflectionClass(A::class);

$defaults = $class->getDefaultProperties();

var_dump($defaults);
var_dump(array_key_exists('c', $defaults));
?>
--EXPECT--
array(0) {
}
bool(false)

