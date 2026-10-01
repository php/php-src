--TEST--
PFA variation: no call, order of destruction
--FILE--
<?php
class Foo {
    function method($a) {}
}
class Dtor {
    public function __construct(public int $id) {}
    public function __destruct() {
        echo __METHOD__, " ", $this->id, "\n";
    }
}
$foo = new Foo;
$f = $foo->method(?);
$g = $f(?);

$map = new WeakMap();
$map[$f] = new Dtor(1);
$map[$g] = new Dtor(2);

unset($f);
unset($g);

echo "OK";
?>
--EXPECT--
Dtor::__destruct 2
Dtor::__destruct 1
OK
