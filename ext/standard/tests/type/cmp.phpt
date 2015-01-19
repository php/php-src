--TEST--
cmp() comparison function
--FILE--
<?php

function user_cmp($a, $b) {
    if ($a < $b) {
        return -1;
    } else if ($a == $b) {
        return 0;
    } else if ($a > $b) {
        return 1;
    }
}

function sign($x) {
    if ($x < 0) {
        return -1;
    } else if ($x == 0) {
        return 0;
    } else {
        return 1;
    }
}

$values = [1, -1, 0, 1.0, -1.0, 0, 7, 7.0, "7a", "1", TRUE, FALSE, NULL];

foreach ($values as $val1) {
    foreach ($values as $val2) {
        $ret1 = cmp($val1, $val2);
        $ret2 = user_cmp($val1, $val2);
        if (sign($ret1) !== sign($ret2)) {
            die("$ret1 !== $ret2");
        }
    }
}

echo "Test passed.";
--EXPECT--
Test passed.
