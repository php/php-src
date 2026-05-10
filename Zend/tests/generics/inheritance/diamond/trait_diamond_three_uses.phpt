--TEST--
Diamond + trait-use: three uses of the same generic trait fold pairwise into one merged contract
--FILE--
<?php
trait Sink<-X> {
    public function sink(X $v): void { echo var_export($v, true), "\n"; }
}

class C {
    use Sink<int>, Sink<string>, Sink<float>;
}

$r = new ReflectionClass(C::class);
$type = $r->getMethod('sink')->getParameters()[0]->getType();
echo "param: ", (string)$type, "\n";

$c = new C;
$c->sink(1);
$c->sink("x");
$c->sink(3.14);
?>
--EXPECT--
param: string|int|float
1
'x'
3.14
