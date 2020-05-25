--TEST--
adding objects to arrays
--FILE--
<?php

$a = array(1,2,3);

$o = new stdclass;
$o->prop = "value";

try {
    var_dump($a + $o);
} catch (Error $e) {
    echo "\nException: " . $e->getMessage() . "\n";
}

$c = $a + $o;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--
Exception: Unsupported operand types: array + stdClass

Fatal error: Uncaught TypeError: Unsupported operand types: array + stdClass in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
