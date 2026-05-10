--TEST--
Diamond + trait-use: variadic contravariant param merges as union
--FILE--
<?php
trait Sink<-X> {
    public function sink(X ...$xs): void {
        foreach ($xs as $x) echo var_export($x, true), "\n";
    }
}

class C {
    use Sink<int>, Sink<string>;
}

$r = new ReflectionClass(C::class);
$type = $r->getMethod('sink')->getParameters()[0]->getType();
echo "param: ", (string)$type, "\n";
echo "is union: ", ($type instanceof ReflectionUnionType ? "yes" : "no"), "\n";

(new C)->sink(1, "x", 2, "y");
?>
--EXPECT--
param: string|int
is union: yes
1
'x'
2
'y'
