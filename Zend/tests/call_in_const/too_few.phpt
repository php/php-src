--TEST--
ArgumentCountError thrown if constant contains call with too few arguments
--FILE--
<?php
class Example {
    const X = sprintf();

    public static function main() {
        echo "X is " . self::X . "\n";
    }
}
for ($i = 0; $i < 2; $i++) {
    try {
        Example::main();
    } catch (ArgumentCountError $e) {
        printf("Caught %s on line %d\n", $e->getMessage(), $e->getLine());
    }
}
?>
--EXPECT--
Caught sprintf() expects at least 1 parameter, 0 given on line 6
Caught sprintf() expects at least 1 parameter, 0 given on line 6
