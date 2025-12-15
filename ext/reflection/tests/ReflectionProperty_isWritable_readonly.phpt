--TEST--
Test ReflectionProperty::isWritable() readonly
--FILE--
<?php

class A {
    public readonly int $a;
    public readonly int $b;

    public function __construct() {
        $this->a = 42;
    }

    public function __clone() {
        test($this);
        $this->a = 43;
        test($this);
    }
}

function test($instance) {
    $rc = new ReflectionClass($instance);
    foreach ($rc->getProperties() as $rp) {
        echo $rp->getName() . ' from A: ';
        var_dump($rp->isWritable($instance::class, $instance));
    }
}

test(new A);
clone new A;

?>
--EXPECT--
a from A: bool(false)
b from A: bool(true)
a from A: bool(true)
b from A: bool(true)
a from A: bool(false)
b from A: bool(true)
