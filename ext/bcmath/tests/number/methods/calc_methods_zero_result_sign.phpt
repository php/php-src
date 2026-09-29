--TEST--
BcMath\Number calc methods return an unsigned zero when the result truncates to zero
--EXTENSIONS--
bcmath
--FILE--
<?php
$cases = [
    ['div', '-0.001', '1', 0],
    ['div', '0.001', '-1', 0],
    ['div', '-0.001', '10', 2],
    ['div', '-0.001', '0.1', 1],
    ['add', '-0.001', '-0.001', 0],
    ['sub', '-0.001', '0.001', 0],
    ['mul', '-0.001', '1', 0],
    ['pow', '-0.1', 3, 2],
];

foreach ($cases as [$method, $num, $arg, $scale]) {
    $ret = (new BcMath\Number($num))->$method($arg, $scale);
    echo "{$num} {$method} {$arg}: {$ret} ", $ret <=> 0, ' ', var_export($ret == 0, true), "\n";
}

[$quot, $rem] = (new BcMath\Number('-0.001'))->divmod('1', 0);
echo "-0.001 divmod 1: {$quot} ", $quot <=> 0, ' ', var_export($quot == 0, true), "\n";
?>
--EXPECT--
-0.001 div 1: 0 0 true
0.001 div -1: 0 0 true
-0.001 div 10: 0.00 0 true
-0.001 div 0.1: 0.0 0 true
-0.001 add -0.001: 0 0 true
-0.001 sub 0.001: 0 0 true
-0.001 mul 1: 0 0 true
-0.1 pow 3: 0.00 0 true
-0.001 divmod 1: 0 0 true
