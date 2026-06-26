--TEST--
Inheritance: a union type argument substituted into an inherited method's parameter/return normalizes to a builtin union
--FILE--
<?php
abstract class TC<T> {
    public function eq(T $a): bool { return true; }
    public function get(mixed $v): T { return $v; }
}
final class AKT extends TC<string|int> {}

$x = new AKT();
\var_dump($x->eq(5));
\var_dump($x->eq("s"));
\var_dump($x->get(5));
\var_dump($x->get("s"));

echo (string) (new ReflectionMethod(AKT::class, 'eq'))->getParameters()[0]->getType(), "\n";
echo (string) (new ReflectionMethod(AKT::class, 'get'))->getReturnType(), "\n";

try {
    $x->eq([]);
} catch (\TypeError $e) {
    echo "rejected: ", $e->getMessage(), "\n";
}
echo "ok\n";
?>
--EXPECTF--
bool(true)
bool(true)
int(5)
string(1) "s"
string|int
string|int
rejected: TC::eq(): Argument #1 ($a) must be of type string|int, array given, called in %s on line %d
ok
