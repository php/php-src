--TEST--
adding strings to arrays
--FILE--
<?php

$a = array(1,2,3);

$s1 = "some string";

try {
    var_dump($a + $s1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$c = $a + $s1;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--
TypeError: Unsupported operand types: array + string

Fatal error: Uncaught TypeError: Unsupported operand types: array + string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
