--TEST--
min() and max() array long/double fast path preserves comparison behavior
--FILE--
<?php

$cases = [];

$cases["packed long"] = [4, 1, 7, -3, 2];

$long_sparse = [4, 1, 7];
unset($long_sparse[1]);
$long_sparse[5] = -2;
$cases["sparse long"] = $long_sparse;

$la = 4;
$lb = 9;
$cases["long refs"] = [&$la, &$lb, 3];

$cases["packed double"] = [4.5, 1.5, 7.5, -3.5, 2.5];

$dbl_sparse = [4.5, 1.5, 7.5];
unset($dbl_sparse[1]);
$dbl_sparse[5] = -2.5;
$cases["sparse double"] = $dbl_sparse;

$da = 4.5;
$db = 9.5;
$cases["double refs"] = [&$da, &$db, 3.5];

$cases["double equal"] = [2.5, 2.5, 2.5];

$cases["nan first"] = [NAN, 1.0, 2.0];
$cases["nan middle"] = [3.0, NAN, 1.0];
$cases["nan last"] = [3.0, 1.0, NAN];
$cases["inf"] = [INF, -INF, 0.0, 5.0];

$cases["single long"] = [7];
$cases["single double"] = [7.5];

$cases["first non-long"] = ["5", 3, 4];
$cases["fallback after longs"] = [5, 4, "3", 2];
$cases["fallback after doubles"] = [5.5, 4.5, "3", 2.5];
$cases["long then double"] = [5, 4, 2.5, 9];
$cases["double then long"] = [5.5, 4.5, 2, 9];

foreach ($cases as $name => $values) {
    echo "-- $name --\n";
    var_dump(min($values));
    var_dump(max($values));
}

?>
--EXPECT--
-- packed long --
int(-3)
int(7)
-- sparse long --
int(-2)
int(7)
-- long refs --
int(3)
int(9)
-- packed double --
float(-3.5)
float(7.5)
-- sparse double --
float(-2.5)
float(7.5)
-- double refs --
float(3.5)
float(9.5)
-- double equal --
float(2.5)
float(2.5)
-- nan first --
float(1)
float(NAN)
-- nan middle --
float(1)
float(3)
-- nan last --
float(NAN)
float(3)
-- inf --
float(-INF)
float(INF)
-- single long --
int(7)
int(7)
-- single double --
float(7.5)
float(7.5)
-- first non-long --
int(3)
string(1) "5"
-- fallback after longs --
int(2)
int(5)
-- fallback after doubles --
float(2.5)
float(5.5)
-- long then double --
float(2.5)
int(9)
-- double then long --
int(2)
int(9)
