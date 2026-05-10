--TEST--
Traits: trait method's isGeneric and getGenericParameters
--FILE--
<?php
trait T {
    public function f<U : int>(U $x): U { return $x; }
}
class C { use T; }
$rm = (new ReflectionClass('C'))->getMethod('f');
echo $rm->isGeneric() ? "gen\n" : "not\n";
foreach ($rm->getGenericParameters() as $p) {
    echo $p->getName(), "\n";
}
?>
--EXPECT--
gen
U
