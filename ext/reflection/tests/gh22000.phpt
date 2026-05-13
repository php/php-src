--TEST--
GH-22000 - Ensure __isset is not returning a reference in ReflectionProperty::isReadable()
--FILE--
<?php
class TestClass1 {
    public int $a = 1;
    public int $b;
    public int $c;

    public function __construct() {
        unset($this->b);
        unset($this->c);
    }

    public function __isset($name) {
        return $name === 'b';
    }

    public function __get($name) {}
}

class TestClass2 {
    public int $d;
    public int $e;
    public int $f;

    public function __construct() {
        unset($this->e);
        unset($this->f);
    }

    public function &__isset($name) {
        return $name === 'f';
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

test('TestClass1');
test('TestClass2');
?>
--EXPECTF--
a from global:bool(true)
b from global:bool(true)
c from global:bool(false)
d from global:bool(false)
e from global:
Notice: Only variable references should be returned by reference in %s%eext%ereflection%etests%egh22000.php on line %d
bool(false)
f from global:
Notice: Only variable references should be returned by reference in %s%eext%ereflection%etests%egh22000.php on line %d
bool(true)
