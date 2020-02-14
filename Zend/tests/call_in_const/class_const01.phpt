--TEST--
Can call internal functions from class constants
--FILE--
<?php
class Example {
    const X = cos(0);

    public static function main() {
        echo "X is " . self::X . "\n";
    }
}
Example::main();
?>
--EXPECT--
X is 1
