--TEST--
RFC example: covariant Pair/Box with self-referential return type, zip, map, identity
--FILE--
<?php

final readonly class Pair<out L, out R> {
    public function __construct(
        public L $left,
        public R $right,
    ) {}

    public function swap(): Pair<R, L> {
        return new Pair($this->right, $this->left);
    }
}

final readonly class Box<out T> {
    public function __construct(
        public T $value,
    ) {}

    public function map<U>(callable $fn): Box<U> {
        return new Box(($fn)($this->value));
    }

    public function zip<O>(O $value): Box<Pair<T, O>> {
        return new Box(new Pair($this->value, $value));
    }
}

function identity<T>(T $value): T {
    return $value;
}

$greeting = new Box::<string>("hello, world");
$paired   = $greeting->zip::<int>(42);
$swapped  = $paired->value->swap();
$result   = identity::<Pair<int, string>>($swapped);

var_dump($result->left);
var_dump($result->right);
?>
--EXPECT--
int(42)
string(12) "hello, world"
