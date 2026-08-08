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

class TestWithDefault {
    public readonly int $prop = 1;

    public function __construct() {
        try {
            $this->prop = 2;
        } catch (Error $e) {
            echo $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
}

function byRef(&$ref) {}

$test = new Test;
var_dump($test->prop); // Read.
try {
    $test->prop = 2;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->prop += 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->prop++;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    ++$test->prop;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $ref =& $test->prop;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->prop =& $ref;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    byRef($test->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump($test->prop2); // Read.
try {
    $test->prop2[] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->prop2[0][] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump(new TestWithDefault()->prop);

?>
--EXPECT--
int(1)
Cannot modify readonly property Test::$prop
Cannot modify readonly property Test::$prop
Cannot modify readonly property Test::$prop
Cannot modify readonly property Test::$prop
Cannot indirectly modify readonly property Test::$prop
Cannot indirectly modify readonly property Test::$prop
Cannot indirectly modify readonly property Test::$prop
array(0) {
}
Cannot indirectly modify readonly property Test::$prop2
Cannot indirectly modify readonly property Test::$prop2
Error: Cannot modify readonly property TestWithDefault::$prop
int(1)
