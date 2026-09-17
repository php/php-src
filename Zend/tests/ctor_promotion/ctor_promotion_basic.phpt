--TEST--
Constructor promotion (basic example)
--FILE--
<?php

class Point {
    public function __construct(public int $x, public int $y, public int $z) {}
}

$point = new Point(1, 2, 3);

// Check that properties really are typed.
try {
    $point->x = "foo";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Cannot assign string to property Point::$x of type int
