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
Warning: __isset unexpectedly returned a reference! in %s%eext%ereflection%etests%egh22000.php on line %d
bool(false)
c from global:
Warning: __isset unexpectedly returned a reference! in %s%eext%ereflection%etests%egh22000.php on line %d
bool(false)

