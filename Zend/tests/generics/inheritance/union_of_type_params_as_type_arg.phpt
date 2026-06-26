--TEST--
Inheritance: a union (or intersection) of type parameters as a type argument erases each member to its bound
--FILE--
<?php
interface TI<T> {
    public function c(mixed $v): T;
}

final class U<Tl, Tr> implements TI<Tl|Tr> {
    public function c(mixed $v): Tl|Tr { return $v; }
}

$a = new U::<string, int>();
\var_dump($a->c(1));
\var_dump($a->c("h"));
\var_dump($a->c([]));

class A {} class B {}
interface TJ<T> { public function c(mixed $v): T; }
final class V<Tl: A, Tr: B> implements TJ<Tl|Tr> {
    public function c(mixed $v): A { return new A(); }
}
\var_dump((new V::<A, B>())->c(null) instanceof A);
echo "ok\n";
?>
--EXPECT--
int(1)
string(1) "h"
array(0) {
}
bool(true)
ok
