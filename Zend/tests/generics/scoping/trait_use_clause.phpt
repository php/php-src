--TEST--
Scoping: T is visible in trait use clause (no compile error)
--FILE--
<?php
trait Holder<X> {
    public function tag(X $x): X { return $x; }
}
class Box<T : object> {
    use Holder<T>;
}
echo (new ReflectionClass('Box'))->getMethod('tag')->getReturnType()->getName(), "\n";
$b = new Box;
echo get_class($b), "\n";
?>
--EXPECT--
mixed
Box
