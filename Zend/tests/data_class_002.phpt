--TEST--
Data classes can be combined with inheritance and other features.
--FILE--
<?php

data class Point {
    public function __construct(public int $x, public int $y) {}
}

final data class Point3D extends Point {
    public function __construct(int $x, int $y, public int $z) {
        parent::__construct($x, $y);
    }
}

$p2 = new Point3D(1, 2, 3);
$p3 = new Point3D(1, 2, 3);
$p4 = new Point3D(2, 3, 4);

var_dump($p2 === $p3);
var_dump($p2 !== $p3);
var_dump($p2 === $p4);
var_dump($p2 !== $p4);
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
