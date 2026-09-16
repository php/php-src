--TEST--
Explicitly unset property with a-vis still respects set visibility
--FILE--
<?php

class C {
    public private(set) int $a = 1;
    public function __construct() {
        unset($this->a);
    }
}

$c = new C();
try {
    $c->a = 2;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unset($c->a);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot modify private(set) property C::$a from global scope
Error: Cannot unset private(set) property C::$a from global scope
