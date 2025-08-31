--TEST--
Dynamic calls to scope introspection functions are forbidden (misoptimization)
--FILE--
<?php

function test() {
    $i = 1;
    try {
        array_map('extract', [['i' => new stdClass]]);
    } catch (\Error $e) {
        echo $e->getMessage() . "\n";
    }
    $i += 1;
    var_dump($i);
}
test();

?>
--EXPECT--
Cannot call extract() dynamically
int(2)
