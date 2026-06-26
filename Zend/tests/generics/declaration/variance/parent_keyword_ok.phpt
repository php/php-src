--TEST--
Variance: parent<T> from a generic child of a generic parent
--FILE--
<?php
readonly class A<out T> {
    public function __construct(public T $value) {}
}
final readonly class B<out T> extends A<T> {
    public function asParent(): parent<T> { return $this; }
}

$b = new B::<int>(7);
echo $b->asParent()->value, "\n";
?>
--EXPECT--
7
