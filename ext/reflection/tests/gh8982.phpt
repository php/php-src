--TEST--
GH-8982 (Attribute target validation fails when read via ReflectionFunction)
--FILE--
<?php

#[Attribute(Attribute::TARGET_FUNCTION)]
class F
{
}

#[Attribute(Attribute::TARGET_METHOD)]
class M
{
}

class C
{
    #[F]
    #[M]
    public function m()
    {
    }
}

#[F]
#[M]
function f() {}

function test(string $attributeClass, $value) {
    try {
        var_dump((new ReflectionFunction($value))->getAttributes($attributeClass)[0]->newInstance());
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

$m = [new C(), 'm'](...);
$f = f(...);

test(F::class, $f);
test(M::class, $f);
test(F::class, $m);
test(M::class, $m);

?>
--EXPECT--
object(F)#4 (0) {
}
Attribute "M" cannot target function (allowed targets: method)
Attribute "F" cannot target method (allowed targets: function)
object(M)#4 (0) {
}
