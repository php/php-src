--TEST--
Inheritance: a property typed with a generic type (Foo<T>) erases to Foo when inherited by a subclass that binds the type arguments
--FILE--
<?php
interface Typed<out T> {
    public function assert(mixed $v): T;
}
final class StringTyped implements Typed<string> {
    public function assert(mixed $v): string {
        if (\is_string($v)) { return $v; }
        throw new \Exception('x');
    }
}
final class IntTyped implements Typed<int> {
    public function assert(mixed $v): int {
        if (\is_int($v)) { return $v; }
        throw new \Exception('y');
    }
}
class UnionTyped<Tl, Tr> implements Typed<Tl|Tr> {
    public function __construct(
        public Typed<Tl> $l,
        public Typed<Tr> $r,
    ) {}
    public function assert(mixed $v): Tl|Tr {
        try { return $this->l->assert($v); }
        catch (\Exception) { return $this->r->assert($v); }
    }
}
final class ArrayKeyTyped extends UnionTyped<string, int> {
    public function __construct() {
        parent::__construct(new StringTyped(), new IntTyped());
    }
}

$a = new ArrayKeyTyped();
\var_dump($a->assert(1));
\var_dump($a->assert("x"));
try {
    $a->assert([]);
} catch (\Exception $e) {
    echo "caught: ", $e->getMessage(), "\n";
}
echo (string) (new ReflectionProperty(UnionTyped::class, 'l'))->getType(), "\n";
\var_dump($a->l instanceof StringTyped);
echo "ok\n";
?>
--EXPECT--
int(1)
string(1) "x"
caught: y
Typed
bool(true)
ok
