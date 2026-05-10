--TEST--
Traits: trait method declares its own type parameter
--FILE--
<?php
trait T {
    public function f<U : int>(U $x): U { return $x; }
}
class C {
    use T;
}
$rm = (new ReflectionClass('C'))->getMethod('f');
echo $rm->getParameters()[0]->getType()->getName(), "\n";
echo $rm->getReturnType()->getName(), "\n";
?>
--EXPECT--
int
int
