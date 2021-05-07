--TEST--
Generated accessors in property promotion
--FILE--
<?php

class ImmVec {
    public function __construct(
        public float $x = 0.0 { get; private set; },
        public float $y = 0.0 { get; private set; },
        public float $z = 0.0 { get; private set; },
    ) {}
}

$vec = new ImmVec(0.0, 1.0, 2.0);
var_dump($vec->x, $vec->y, $vec->z);

try {
    $vec->x = 42.0;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
float(0)
float(1)
float(2)
Call to private accessor ImmVec::$x::set() from global scope
