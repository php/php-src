--TEST--
Traits: trait use clause with type arguments compiles
--FILE--
<?php
trait Holder<X> {
    public function tag(X $x): X { return $x; }
}
class Box<T : object> {
    use Holder<T>;
}
$b = new Box;
echo get_class($b), "\n";
echo "ok\n";
?>
--EXPECT--
Box
ok
