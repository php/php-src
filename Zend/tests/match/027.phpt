--TEST--
Test result of match cannot be modified by reference
--FILE--
<?php

// opcache can't be certain Test::usesRef is actually this method
if (!class_exists('Test')) {
    class Test {
        public static function usesRef(&$x) {
            $x = 'modified';
        }
        public static function usesValue($x) {
            echo "usesValue $x\n";
        }
    }
}

function main() {
    $i = 0;
    Test::usesValue(match(true) { true => $i });
    echo "i is $i\n";
    $j = 1;
    Test::usesRef(match(true) { true => $j });
    echo "j is $j\n";
}
main();

?>
--EXPECTF--
usesValue 0
i is 0

Fatal error: Uncaught Error: Cannot pass parameter 1 by reference in %s027.php:20
Stack trace:
#0 %s027.php(23): main()
#1 {main}
  thrown in %s027.php on line 20
