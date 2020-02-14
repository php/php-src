--TEST--
ArgumentCountError thrown if constant contains call with too few arguments
--FILE--
<?php
class Example {
    const X = intdiv();

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
Caught intdiv() expects exactly 2 parameters, 0 given on line 6
Caught intdiv() expects exactly 2 parameters, 0 given on line 6
