--TEST--
Readonly property with default value and asymmetric visibility
--FILE--
<?php

class Test {
    public readonly int $default = 1;
    public private(set) readonly int $private = 2;
    public protected(set) readonly int $protected = 3;
    public public(set) readonly int $public = 4;
}

$test = new Test();

foreach (['default', 'private', 'protected', 'public'] as $prop) {
    $before = $test->$prop;
    try {
        $test->$prop = 42;
    } catch (Error $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
    echo "$$prop before $before, after {$test->$prop}", PHP_EOL;
}

?>
--EXPECT--
Error: Cannot modify readonly property Test::$default
$default before 1, after 1
Error: Cannot modify readonly property Test::$private
$private before 2, after 2
Error: Cannot modify readonly property Test::$protected
$protected before 3, after 3
Error: Cannot modify readonly property Test::$public
$public before 4, after 4
