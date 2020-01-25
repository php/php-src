--TEST--
Can call internal functions from class constants
--FILE--
<?php
class Example {
    const X = sprintf("Hello, %s\n", "World");

    public static function main() {
        echo "X is " . self::X . "\n";
    }
}
Example::main();
?>
--EXPECT--
X is Hello, World
