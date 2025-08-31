--TEST--
clamp() tests
--INI--
precision=14
date.timezone = UTC
--FILE--
<?php

var_dump(clamp(2, 1, 3));
var_dump(clamp(0, 1, 3));
var_dump(clamp(6, 1, 3));
var_dump(clamp(2, 1.3, 3.4));
var_dump(clamp(2.5, 1, 3));
var_dump(clamp(2.5, 1.3, 3.4));
var_dump(clamp(0, 1.3, 3.4));
var_dump(clamp(M_PI, -INF, INF));
var_dump(clamp(NAN, 4, 6));
var_dump(clamp("a", "c", "g"));
var_dump(clamp("d", "c", "g"));
echo clamp('2025-08-01', '2025-08-15', '2025-09-15'), "\n";
echo clamp('2025-08-20', '2025-08-15', '2025-09-15'), "\n";
echo clamp(new \DateTimeImmutable('2025-08-01'), new \DateTimeImmutable('2025-08-15'), new \DateTimeImmutable('2025-09-15'))->format('Y-m-d'), "\n";
echo clamp(new \DateTimeImmutable('2025-08-20'), new \DateTimeImmutable('2025-08-15'), new \DateTimeImmutable('2025-09-15'))->format('Y-m-d'), "\n";

try {
    var_dump(clamp(4, NAN, 6));
} catch (ValueError $error) {
    echo $error->getMessage(), "\n";
}

try {
    var_dump(clamp(7, 6, NAN));
} catch (ValueError $error) {
    echo $error->getMessage(), "\n";
}

try {
    var_dump(clamp(1, 3, 2));
} catch (ValueError $error) {
    echo $error->getMessage(), "\n";
}
?>
--EXPECT--
int(2)
int(1)
int(3)
int(2)
float(2.5)
float(2.5)
float(1.3)
float(3.141592653589793)
float(NAN)
string(1) "c"
string(1) "d"
2025-08-15
2025-08-20
2025-08-15
2025-08-20
clamp(): Argument #2 ($min) cannot be NAN
clamp(): Argument #3 ($max) cannot be NAN
clamp(): Argument #2 ($min) must be smaller than or equal to argument #3 ($max)
