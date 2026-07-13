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
var_dump($test->default, $test->private, $test->protected, $test->public);

foreach (['default', 'private', 'protected', 'public'] as $prop) {
    try {
        $test->$prop = 42;
    } catch (Error $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
Error: Cannot modify readonly property Test::$default
Error: Cannot modify readonly property Test::$private
Error: Cannot modify readonly property Test::$protected
Error: Cannot modify readonly property Test::$public
