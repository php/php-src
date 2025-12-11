--TEST--
clamp() tests
--INI--
precision=14
date.timezone=UTC
zend.assertions=1
--FILE--
<?php

function make_clamp_fcc() {
    return clamp(...);
}

function check_clamp_result($value, $min, $max) {
    $flf = clamp($value, $min, $max);
    $dyn = make_clamp_fcc()($value, $min, $max);
    assert($flf === $dyn || (is_nan($flf) && is_nan($dyn)));

    return $flf;
}

function check_clamp_exception($value, $min, $max) {
    try {
        var_dump(clamp($value, $min, $max));
    } catch (ValueError $error) {
        echo $error->getMessage(), "\n";
    }

    try {
        var_dump(make_clamp_fcc()($value, $min, $max));
    } catch (ValueError $error) {
        echo $error->getMessage(), "\n";
    }
}

var_dump(check_clamp_result(2, 1, 3));
var_dump(check_clamp_result(0, 1, 3));
var_dump(check_clamp_result(6, 1, 3));
var_dump(check_clamp_result(2, 1.3, 3.4));
var_dump(check_clamp_result(2.5, 1, 3));
var_dump(check_clamp_result(2.5, 1.3, 3.4));
var_dump(check_clamp_result(0, 1.3, 3.4));
var_dump(check_clamp_result(M_PI, -INF, INF));
var_dump(check_clamp_result(NAN, 4, 6));
var_dump(check_clamp_result("a", "c", "g"));
var_dump(check_clamp_result("d", "c", "g"));
echo check_clamp_result('2025-08-01', '2025-08-15', '2025-09-15'), "\n";
echo check_clamp_result('2025-08-20', '2025-08-15', '2025-09-15'), "\n";
echo check_clamp_result(new \DateTimeImmutable('2025-08-01'), new \DateTimeImmutable('2025-08-15'), new \DateTimeImmutable('2025-09-15'))->format('Y-m-d'), "\n";
echo check_clamp_result(new \DateTimeImmutable('2025-08-20'), new \DateTimeImmutable('2025-08-15'), new \DateTimeImmutable('2025-09-15'))->format('Y-m-d'), "\n";
var_dump(check_clamp_result(null, -1, 1));
var_dump(check_clamp_result(null, 1, 3));
var_dump(check_clamp_result(null, -3, -1));
var_dump(check_clamp_result(-9999, null, 10));
var_dump(check_clamp_result(12, null, 10));

$a = new \InvalidArgumentException('a');
$b = new \RuntimeException('b');
$c = new \LogicException('c');
echo check_clamp_result($a, $b, $c)::class, "\n";
echo check_clamp_result($b, $a, $c)::class, "\n";
echo check_clamp_result($c, $a, $b)::class, "\n";

check_clamp_exception(4, NAN, 6);
check_clamp_exception(7, 6, NAN);
check_clamp_exception(1, 3, 2);
check_clamp_exception(-9999, 5, null);
check_clamp_exception(12, -5, null);

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
int(-1)
int(1)
int(-3)
int(-9999)
int(10)
InvalidArgumentException
RuntimeException
LogicException
clamp(): Argument #2 ($min) must not be NAN
clamp(): Argument #2 ($min) must not be NAN
clamp(): Argument #3 ($max) must not be NAN
clamp(): Argument #3 ($max) must not be NAN
clamp(): Argument #2 ($min) must be smaller than or equal to argument #3 ($max)
clamp(): Argument #2 ($min) must be smaller than or equal to argument #3 ($max)
clamp(): Argument #2 ($min) must be smaller than or equal to argument #3 ($max)
clamp(): Argument #2 ($min) must be smaller than or equal to argument #3 ($max)
clamp(): Argument #2 ($min) must be smaller than or equal to argument #3 ($max)
clamp(): Argument #2 ($min) must be smaller than or equal to argument #3 ($max)
