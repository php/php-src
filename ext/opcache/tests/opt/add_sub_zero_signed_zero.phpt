--TEST--
DFA pass must not drop a float + 0.0 that turns -0.0 into 0.0
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php

function add_int_zero(float $x) { return $x + 0; }
function add_zero_left(float $x) { return 0 + $x; }
function add_float_zero(float $x) { return $x + 0.0; }
function add_float_zero_left(float $x) { return 0.0 + $x; }
function sub_neg_zero(float $x) { return $x - -0.0; }
function round_add_zero($x) { return round($x, 2) + 0.0; }
function cast_add_zero($x) { return 0 + (float) $x; }

class C { public float $p = -0.0; }
function prop_add_zero(C $c) { return $c->p + 0; }

/* These were already correct and must stay so */
function sub_zero(float $x) { return $x - 0.0; }
function sub_int_zero(float $x) { return $x - 0; }
function add_neg_zero(float $x) { return $x + -0.0; }
function add_neg_zero_left(float $x) { return -0.0 + $x; }
function int_add_zero(int $x) { return $x + 0; }

foreach ([
    'add_int_zero', 'add_zero_left', 'add_float_zero', 'add_float_zero_left', 'sub_neg_zero',
    'sub_zero', 'sub_int_zero', 'add_neg_zero', 'add_neg_zero_left',
] as $f) {
    echo $f, ': ';
    var_dump($f(-0.0), $f(0.0), $f(1.5));
}
echo "round_add_zero: "; var_dump(round_add_zero(-0.004));
echo "cast_add_zero: "; var_dump(cast_add_zero("-0"));
echo "prop_add_zero: "; var_dump(prop_add_zero(new C));
echo "int_add_zero: "; var_dump(int_add_zero(-3));
echo json_encode(round_add_zero(-0.004)), "\n";

?>
--EXPECT--
add_int_zero: float(0)
float(0)
float(1.5)
add_zero_left: float(0)
float(0)
float(1.5)
add_float_zero: float(0)
float(0)
float(1.5)
add_float_zero_left: float(0)
float(0)
float(1.5)
sub_neg_zero: float(0)
float(0)
float(1.5)
sub_zero: float(-0)
float(0)
float(1.5)
sub_int_zero: float(-0)
float(0)
float(1.5)
add_neg_zero: float(-0)
float(0)
float(1.5)
add_neg_zero_left: float(-0)
float(0)
float(1.5)
round_add_zero: float(0)
cast_add_zero: float(0)
prop_add_zero: float(0)
int_add_zero: int(-3)
0
