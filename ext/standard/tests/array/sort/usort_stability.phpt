--TEST--
usort() is stable
--FILE--
<?php

$array = range(0, 10000);
usort($array, function($a, $b) {
    // Everything is equal!
    return 0;
});
var_dump($array === range(0, 10000));

?>
--EXPECT--
bool(true)
