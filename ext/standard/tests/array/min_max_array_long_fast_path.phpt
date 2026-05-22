--TEST--
min() and max() array long fast path preserves comparison behavior
--FILE--
<?php

$cases = [];

$cases["packed"] = [4, 1, 7, -3, 2];

$sparse = [4, 1, 7];
unset($sparse[1]);
$sparse[5] = -2;
$cases["sparse"] = $sparse;

$a = 4;
$b = 9;
$cases["refs"] = [&$a, &$b, 3];

$cases["first non-long"] = ["5", 3, 4];
$cases["fallback after longs"] = [5, 4, "3", 2];

foreach ($cases as $name => $values) {
    echo "-- $name --\n";
    var_dump(min($values));
    var_dump(max($values));
}

?>
--EXPECT--
-- packed --
int(-3)
int(7)
-- sparse --
int(-2)
int(7)
-- refs --
int(3)
int(9)
-- first non-long --
int(3)
string(1) "5"
-- fallback after longs --
int(2)
int(5)
