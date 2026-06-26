--TEST--
Generic syntax: method declaration with type parameters
--FILE--
<?php
class C {
    public function map<U>(callable $f): U { return $f(); }
}
$r = (new ReflectionClass('C'))->getMethod('map');
var_dump($r->isGeneric());
echo $r->getGenericParameters()[0]->getName(), "\n";
?>
--EXPECT--
bool(true)
U
