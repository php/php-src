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
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    unset($c->a);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot modify private(set) property C::$a from global scope
Cannot unset private(set) property C::$a from global scope
