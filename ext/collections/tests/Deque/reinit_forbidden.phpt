--TEST--
Collections\Deque cannot be re-initialized
--FILE--
<?php

$it = new Collections\Deque([]);

try {
    $it->__construct(['first']);
    echo "Unexpectedly called constructor\n";
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
var_dump($it);
try {
    $it->__unserialize([new ArrayObject(), new stdClass()]);
    echo "Unexpectedly called __unserialize\n";
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
var_dump($it);
?>
--EXPECT--
Caught RuntimeException: Called Collections\Deque::__construct twice
object(Collections\Deque)#1 (0) {
}
Caught RuntimeException: Already unserialized
object(Collections\Deque)#1 (0) {
}