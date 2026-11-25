--TEST--
GH-25744: Adding zero must preserve floating-point arithmetic semantics
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function add_int(float $x) { return $x + 0; }
function add_int_left(float $x) { return 0 + $x; }
function add_float(float $x) { return $x + 0.0; }
function add_float_left(float $x) { return 0.0 + $x; }
function sub_negative_zero(float $x) { return $x - -0.0; }
function add_negative_zero(float $x) { return $x + -0.0; }
function add_negative_zero_left(float $x) { return -0.0 + $x; }
function sub_float(float $x) { return $x - 0.0; }
function sub_int(float $x) { return $x - 0; }

function integer_ops(int $x) { return [$x + 0, 0 + $x, $x - 0]; }
function rounded($x) { return round($x, 2) + 0.0; }
function ceiling($x) { return ceil($x) + 0.0; }
function flooring($x) { return floor($x) + 0.0; }
function casted($x) { $f = (float) $x; return (string) ($f + 0.0); }

foreach ([
    'add_int', 'add_int_left', 'add_float', 'add_float_left', 'sub_negative_zero',
    'add_negative_zero', 'add_negative_zero_left', 'sub_float', 'sub_int',
] as $fn) {
    // Exercise hot functions as well as the optimizer without a JIT.
    for ($i = 0; $i < 200; $i++) {
        $results = [];
        foreach ([-0.0, 0.0, 1.5, -1.5, INF, -INF, NAN] as $x) {
            $value = $fn($x);
            $results[] = serialize($value)
                . ($value == 0.0 ? '/' . fdiv(1.0, $value) : '');
        }
    }
    echo $fn, ': ', implode(' ', $results), "\n";
}

foreach ([-3, 0, 3] as $x) {
    echo serialize(integer_ops($x)), "\n";
}
var_dump(casted('-0'));
echo json_encode(['v' => rounded(-0.001)]), "\n";
echo serialize(ceiling(-0.5)), "\n";
echo serialize(flooring(-0.0)), "\n";
?>
--EXPECT--
add_int: d:0;/INF d:0;/INF d:1.5; d:-1.5; d:INF; d:-INF; d:NAN;
add_int_left: d:0;/INF d:0;/INF d:1.5; d:-1.5; d:INF; d:-INF; d:NAN;
add_float: d:0;/INF d:0;/INF d:1.5; d:-1.5; d:INF; d:-INF; d:NAN;
add_float_left: d:0;/INF d:0;/INF d:1.5; d:-1.5; d:INF; d:-INF; d:NAN;
sub_negative_zero: d:0;/INF d:0;/INF d:1.5; d:-1.5; d:INF; d:-INF; d:NAN;
add_negative_zero: d:-0;/-INF d:0;/INF d:1.5; d:-1.5; d:INF; d:-INF; d:NAN;
add_negative_zero_left: d:-0;/-INF d:0;/INF d:1.5; d:-1.5; d:INF; d:-INF; d:NAN;
sub_float: d:-0;/-INF d:0;/INF d:1.5; d:-1.5; d:INF; d:-INF; d:NAN;
sub_int: d:-0;/-INF d:0;/INF d:1.5; d:-1.5; d:INF; d:-INF; d:NAN;
a:3:{i:0;i:-3;i:1;i:-3;i:2;i:-3;}
a:3:{i:0;i:0;i:1;i:0;i:2;i:0;}
a:3:{i:0;i:3;i:1;i:3;i:2;i:3;}
string(1) "0"
{"v":0}
d:0;
d:0;
