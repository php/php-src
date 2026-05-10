--TEST--
Reflection: __toString on type parameter
--FILE--
<?php
class A<X, Y : object, Z = int, out W : object = stdClass> {}
foreach ((new ReflectionClass('A'))->getGenericParameters() as $p) {
    echo $p, "\n";
}
?>
--EXPECT--
X
Y : object
Z = int
out W : object = stdClass
