--TEST--
Traits: class uses multiple generic traits
--FILE--
<?php
trait A<X : object> {
    public function a(): X { return $this->ax; }
    public X $ax;
}
trait B<Y : int> {
    public function b(): Y { return $this->by; }
    public Y $by;
}
class C {
    use A<object>, B<int>;
}
$rc = new ReflectionClass('C');
echo $rc->getMethod('a')->getReturnType()->getName(), "\n";
echo $rc->getMethod('b')->getReturnType()->getName(), "\n";
?>
--EXPECT--
object
int
