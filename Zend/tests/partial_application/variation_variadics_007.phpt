--TEST--
PFA variation: extra through variadic
--FILE--
<?php
function appliesCb($cb) { return runsCb($cb(1, ?, ...)); }

function runsCb($cb) {
        return $cb(2, 3);
}

var_dump(appliesCb(fn($a, $b) => $a + $b));
?>
--EXPECT--
int(3)
