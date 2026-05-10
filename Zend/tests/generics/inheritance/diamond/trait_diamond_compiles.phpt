--TEST--
Diamond + trait-use: multiple uses of the same generic trait with different bindings merge use-site-variantly at the reflection level
--FILE--
<?php
trait Setter<in X> {
    public function set(X $v): void { echo var_export($v, true), "\n"; }
}

class C {
    use Setter<int>, Setter<string>;
}

$r = new ReflectionClass(C::class);
$type = $r->getMethod('set')->getParameters()[0]->getType();
echo "param: ", (string)$type, "\n";
echo "is union: ", ($type instanceof ReflectionUnionType ? "yes" : "no"), "\n";

$c = new C();
$c->set(1);
$c->set("hello");
?>
--EXPECT--
param: string|int
is union: yes
1
'hello'
