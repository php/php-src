--TEST--
Generator wit type check
--FILE--
<?php
function gen(array $a) { yield; }
try {
    gen(42);
} catch (TypeError $e) {
    echo $e->getMessage()."\n";
}

try {
    foreach (gen(42) as $val) {
        var_dump($val);
    }
} catch (TypeError $e) {
        echo $e->getMessage()."\n";
}
?>
--EXPECTF--
gen() expects argument #1 ($a) to be of type array, int given, called in %s on line %d
gen() expects argument #1 ($a) to be of type array, int given, called in %s on line %d
