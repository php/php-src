--TEST--
GC 050: Try/finally in foreach should create separate live ranges
--FILE--
<?php

function f(int $n): object {
    try {
        foreach ((array) $n as $v) {
            if ($n === 1) {
                try {
                    $a = new stdClass;
                    return $a;
                } finally {
                    return $ret = $a;
                }
            }
            if ($n === 2) {
                $b = new stdClass;
                return $ret = $b;
            }
        }
    } finally {
        $ret->v = 1;
    }
    return new stdClass;
}

for ($i = 0; $i < 100000; $i++) {
    // Create cyclic garbage to trigger GC
    $a = new stdClass;
    $b = new stdClass;
    $a->r = $b;
    $b->r = $a;

    $r = f($i % 2 + 1);
}
echo "OK\n";
?>
--EXPECT--
OK
