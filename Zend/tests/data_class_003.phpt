--TEST--
Data class inheritance rules are enforced.
--FILE--
<?php

class Point {
    public function __construct(public int $x, public int $y) {}
}

data class Point3D extends Point {
    public function __construct(int $x, int $y, public int $z) {
        parent::__construct($x, $y);
    }
}
?>
--EXPECTF--
Fatal error: Data class Point3D cannot extend non-data class Point in %s on line %d
