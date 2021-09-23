--TEST--
Vector cannot be re-initialized
--FILE--
<?php

$vec = new Vector([]);

try {
    $vec->__construct(['first']);
    echo "Unexpectedly called constructor\n";
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
var_dump($vec);
try {
    $vec->__unserialize([new ArrayObject(), new stdClass()]);
    echo "Unexpectedly called __unserialize\n";
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
var_dump($vec);
?>
--EXPECT--
Caught RuntimeException: Called Vector::__construct twice
object(Vector)#1 (0) {
}
Caught RuntimeException: Already unserialized
object(Vector)#1 (0) {
}