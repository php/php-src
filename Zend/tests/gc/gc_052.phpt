--TEST--
GC 049: Multiple early returns from foreach should create separate live ranges
--FILE--
<?php

function f(int $n): object {
    foreach ((array) $n as $v) {
        if ($n === 1) {
            $a = new stdClass;
            return $a;
        }
        if ($n === 2) {
            $b = new stdClass;
            return $b;
        }
        if ($n === 3) {
            $c = new stdClass;
            return $c;
        }
    }
    return new stdClass;
}

for ($i = 0; $i < 100000; $i++) {
    // Create cyclic garbage to trigger GC
    $a = new stdClass;
    $b = new stdClass;
    $a->r = $b;
    $b->r = $a;

    $r = f($i % 3 + 1);
}
echo "OK\n";
?>
--EXPECT--
OK
