--TEST--
Evaluation order of initializers in class declaration
--FILE--
<?php

class Order {
    public function __construct(int $i) {
        echo "Creating $i\n";
    }
}

class A {
    const C1 = new Order(1);
    public static $s1 = new Order(3);
    public $p1 = new Order(10);
    const C2 = new Order(2);
    public static $s2 = new Order(4);
    public $p2 = new Order(11);
}
class B extends A {
    const C3 = new Order(5);
    public static $s3 = new Order(6);
    public $p3 = new Order(12);
}

echo "\n";
new A;
echo "\n";
new B;

?>
--EXPECT--
Creating 1
Creating 2
Creating 3
Creating 4
Creating 5
Creating 6

Creating 10
Creating 11

Creating 10
Creating 11
Creating 12
