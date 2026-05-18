--TEST--
Traits: ReflectionClass::getGenericParameters on trait
--FILE--
<?php
trait T<A, B : object, C = int> {}
$rc = new ReflectionClass('T');
foreach ($rc->getGenericParameters() as $p) {
    echo $p->getName(), "\n";
}
?>
--EXPECT--
A
B
C
