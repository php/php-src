--TEST--
Test error operation of random_bytes()
--FILE--
<?php
//-=-=-=-

var_dump(random_bytes());

$bytes = null;
try {
    $bytes = random_bytes(0);
} catch (Exception $e) {
    var_dump($e->getMessage());
}
var_dump($bytes);

?>
--EXPECTF--

Warning: random_bytes() expects exactly 1 parameter, 0 given in %s on line %d
NULL
string(29) "Length must be greater than 0"
NULL
