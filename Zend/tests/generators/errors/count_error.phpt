--TEST--
Generators can't be counted
--FILE--
<?php

function gen() { yield; }

$gen = gen();

try {
    count($gen);
} catch (Exception $e) {
    echo $e;
} catch (\TypeError $e) {
     echo $e->getMessage() . "\n";
 }

?>
--EXPECT--
Parameter must be an array or an object that implements Countable
