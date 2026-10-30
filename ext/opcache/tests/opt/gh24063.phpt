--TEST--
GH-24063 (SCCP merges 0.0 and -0.0 into a single constant)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php

function scalar(float $n): float
{
    $r = $n < 0 ? -0.0 : 0.0;
    return $r;
}

function arr(float $n): array
{
    $r = $n < 0 ? [-0.0] : [0.0];
    return $r;
}

function partial(float $n, $x): float
{
    $a = ['x' => $x];
    if ($n < 0) {
        $a['k'] = -0.0;
    } else {
        $a['k'] = 0.0;
    }
    return $a['k'];
}

var_dump(scalar(1.0), scalar(-1.0));
var_dump(arr(1.0)[0], arr(-1.0)[0]);
var_dump(partial(1.0, 1), partial(-1.0, 1));

?>
--EXPECT--
float(0)
float(-0)
float(0)
float(-0)
float(0)
float(-0)
