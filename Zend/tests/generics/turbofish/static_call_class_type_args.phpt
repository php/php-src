--TEST--
Turbofish: class type arguments on a static call (Class::<Args>::method())
--FILE--
<?php
class Option<T: string|int> {
    private mixed $v;
    public function __construct(T $v) { $this->v = $v; }

    public static function some(T $v): static {
        return new static($v);
    }
    public function get(): T { return $this->v; }
}

$a = Option::<string>::some("hello");
$b = Option::<int>::some(42);
\var_dump($a->get(), $b->get());

$c = Option::some("bare");
\var_dump($c->get());

class Pair<A, B> {
    public static function make(A $a, B $b): string {
        return get_debug_type($a) . "," . get_debug_type($b);
    }
}
echo Pair::<int, string>::make(1, "x"), "\n";
echo "ok\n";
?>
--EXPECT--
string(5) "hello"
int(42)
string(4) "bare"
int,string
ok
