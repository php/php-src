--TEST--
Test ReflectionProperty::isReadable() init
--FILE--
<?php

class A {
    public $a;
    public int $b;
    public int $c = 42;
    public int $d;
    public int $e;

    public function __construct() {
        unset($this->e);
    }
}

class B {
    public int $f;
    public int $g;
    public int $h;

    public function __construct() {
        unset($this->g);
        unset($this->h);
    }

    public function __isset($name) {
        return $name === 'h';
    }

    public function __get($name) {}
}

class C {
    public int $i;
    public int $j;
    public int $k;

    public function __construct() {
        unset($this->j);
        unset($this->k);
    }

    public function __get($name) {}
}

function test($class) {
    $rc = new ReflectionClass($class);
    foreach ($rc->getProperties() as $rp) {
        echo $rp->getName() . ' from global: ';
        var_dump($rp->isReadable(null, new $class));
    }
}

test('A');
test('B');
test('C');

?>
--EXPECT--
a from global: bool(true)
b from global: bool(false)
c from global: bool(true)
d from global: bool(false)
e from global: bool(false)
f from global: bool(false)
g from global: bool(false)
h from global: bool(true)
i from global: bool(false)
j from global: bool(true)
k from global: bool(true)
