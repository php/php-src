--TEST--
Modifying a readonly property
--FILE--
<?php

class Test {
    readonly public int $prop;
    readonly public array $prop2;

    public function __construct() {
        // Initializing assignments.
        $this->prop = 1;
        $this->prop2 = [];
    }
}

function byRef(&$ref) {}

$test = new Test;
var_dump($test->prop); // Read.
try {
    $test->prop = 2;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $test->prop += 1;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $test->prop++;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    ++$test->prop;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $ref =& $test->prop;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $test->prop =& $ref;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    byRef($test->prop);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($test->prop2); // Read.
try {
    $test->prop2[] = 1;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $test->prop2[0][] = 1;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
int(1)
Error: Cannot modify readonly property Test::$prop
Error: Cannot modify readonly property Test::$prop
Error: Cannot modify readonly property Test::$prop
Error: Cannot modify readonly property Test::$prop
Error: Cannot indirectly modify readonly property Test::$prop
Error: Cannot indirectly modify readonly property Test::$prop
Error: Cannot indirectly modify readonly property Test::$prop
array(0) {
}
Error: Cannot indirectly modify readonly property Test::$prop2
Error: Cannot indirectly modify readonly property Test::$prop2
