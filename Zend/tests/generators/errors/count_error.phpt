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
}

?>
--EXPECTF--
Exception: Counting of 'Generator' is not allowed in %s:%d
Stack trace:
#0 %s(%d): count(Object(Generator))
#1 {main}
