--TEST--
adding arrays to objects
--FILE--
<?php

$a = array(1,2,3);

$o = new stdclass;
$o->prop = "value";

try {
    var_dump($o + $a);
} catch (Error $e) {
    echo "\nException: " . $e->getMessage() . "\n";
}

$c = $o + $a;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--
Exception: Unsupported operand types: stdClass + array

Fatal error: Uncaught TypeError: Unsupported operand types: stdClass + array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
