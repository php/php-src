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
    try {
        $rc = new ReflectionClass($class);
        foreach ($rc->getProperties() as $rp) {
            echo $rp->getName() . ' from global: ';
            var_dump($rp->isReadable(null, new $class));
        }
    } catch(\Exception $ex) {
        echo $ex->getMessage();
    }
}

test('TestClass');
?>
--EXPECTF--
a from global: bool(false)
b from global: __isset should not return a reference
