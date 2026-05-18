--TEST--
Variance: static<T> in a covariant return position
--FILE--
<?php
final readonly class C<+T> {
    public function __construct(public T $value) {}
    public function dup(): static<T> { return new static($this->value); }
}

$c = new C::<string>("ok");
echo $c->dup()->value, "\n";
?>
--EXPECT--
ok
