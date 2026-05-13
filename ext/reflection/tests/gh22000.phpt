--TEST--
GH-22000 - Ensure __isset is not returning a reference in ReflectionProperty::isReadable()
--FILE--
<?php
class TestClass {
    public int $a;
    public int $b;
    public int $c;

    public function __construct() {
        unset($this->b);
        unset($this->c);
    }

    public function &__isset($name) {
        return $name === 'c';
    }

    public function __get($name) {}
}


function test($class) {
    $rc = new ReflectionClass($class);
    foreach ($rc->getProperties() as $rp) {
        echo $rp->getName() . ' from global:';
        var_dump($rp->isReadable(null, new $class));
    }
}

test('TestClass');
?>
--EXPECTF--
a from global:bool(false)
b from global:
Notice: Only variable references should be returned by reference in /home/zaaarf/dev/irl/c/php/ext/reflection/tests/gh22000.php on line 13
bool(false)
c from global:
Notice: Only variable references should be returned by reference in /home/zaaarf/dev/irl/c/php/ext/reflection/tests/gh22000.php on line 13
bool(true)
