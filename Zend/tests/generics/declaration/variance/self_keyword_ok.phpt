--TEST--
Variance: self<T> in a covariant return position
--FILE--
<?php
final readonly class C<out T> {
    public function __construct(public T $value) {}
    public function clone_(): self<T> { return new self($this->value); }
}

$c = new C::<int>(42);
echo $c->clone_()->value, "\n";
?>
--EXPECT--
42
