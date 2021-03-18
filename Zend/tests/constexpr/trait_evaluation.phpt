--TEST--
Initializers in traits should not be eagerly evaluated
--FILE--
<?php

class Order {
    public function __construct(string $class, int $i) {
        echo "Creating $class($i)\n";
    }
}

trait T {
    public static $s = new Order(self::class, 1);
    public $p = new Order(self::class, 2);
}

class C {
    use T;
}
new C;

// Lazy-evaluate illegal direct trait access.
T::$s;

?>
--EXPECT--
Creating C(1)
Creating C(2)
Creating T(1)
