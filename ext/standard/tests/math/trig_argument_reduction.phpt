--TEST--
cos/sin/tan argument reduction is not broken by the FPU precision control word
--FILE--
<?php
$cases = [
    ['cos', 10.0, -0.8390715290764524], ['cos', 100.0, 0.8623188722876839],
    ['sin', 10.0, -0.5440211108893698], ['sin', 700.0, 0.5439705233633756],
    ['tan', 10.0, 0.6483608274590866],
];
foreach ($cases as [$fn, $x, $want]) {
    $got = $fn($x);
    echo $fn, '(', $x, '): ', abs($got - $want) <= 2e-16 * max(1.0, abs($want)) ? 'ok' : "got $got", "\n";
}
?>
--EXPECT--
cos(10): ok
cos(100): ok
sin(10): ok
sin(700): ok
tan(10): ok
