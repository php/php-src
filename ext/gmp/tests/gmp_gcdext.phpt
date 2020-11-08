--TEST--
gmp_gcdext() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip";
?>
--FILE--
<?php

$n = gmp_init("34293864345");
$n1 = gmp_init("23434293864345");

$a = array(
    array(123,45),
    array(4341,9734),
    array(23487,333),
    array(-234234,-123123),
    array(-100,-2234),
    array(345,"34587345"),
    array(345,"0"),
    array("345556456",345873),
    array("34545345556456","323432445873"),
    array($n, $n1),
    );

foreach ($a as $val) {
    $r = gmp_gcdext($val[0],$val[1]);
    $check = gmp_add(gmp_mul($val[0],$r['s']), gmp_mul($val[1],$r['t']));
    var_dump(gmp_strval($r['g']));
    var_dump(gmp_strval($check));
}

try {
    var_dump(gmp_gcdext($val[0], array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_gcdext(array(), array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
string(1) "3"
string(1) "3"
string(1) "1"
string(1) "1"
string(1) "3"
string(1) "3"
string(4) "3003"
string(4) "3003"
string(1) "2"
string(1) "2"
string(2) "15"
string(2) "15"
string(3) "345"
string(3) "345"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(3) "195"
string(3) "195"
gmp_gcdext(): Argument #2 ($num2) must be of type GMP|string|int, array given
gmp_gcdext(): Argument #1 ($num1) must be of type GMP|string|int, array given
Done
