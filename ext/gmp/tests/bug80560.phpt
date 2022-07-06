--TEST--
Bug #80560: Strings containing only a base prefix return 0 object
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

$functions1 = [
    'gmp_init',
    'gmp_export',
    'gmp_intval',
    'gmp_strval',
    'gmp_neg',
    'gmp_abs',
    'gmp_fact',
    'gmp_sqrt',
    'gmp_sqrtrem',
    'gmp_perfect_square',
    'gmp_perfect_power',
    'gmp_prob_prime',
    'gmp_sign',
    'gmp_random_seed',
    'gmp_popcount',
    'gmp_com',
    'gmp_nextprime',
];
$functions1_need_int_2 = [
    'gmp_testbit',
    'gmp_scan0',
    'gmp_scan1',
    'gmp_binomial',
    'gmp_root',
    'gmp_rootrem',
    'gmp_pow',
];
$functions2 = [
    'gmp_add',
    'gmp_sub',
    'gmp_mul',
    'gmp_div',
    'gmp_div_q',
    'gmp_div_r',
    'gmp_div_qr',
    'gmp_divexact',
    'gmp_mod',
    'gmp_gcd',
    'gmp_gcdext',
    'gmp_lcm',
    'gmp_invert',
    'gmp_jacobi',
    'gmp_legendre',
    'gmp_kronecker',
    'gmp_cmp',
    'gmp_random_range',
    'gmp_and',
    'gmp_or',
    'gmp_xor',
    'gmp_hamdist',
];
$functions3 = [
    'gmp_powm',
];

echo 'Explicit base with gmp_init:', \PHP_EOL;
echo 'Hexadecimal', \PHP_EOL;
try {
    var_dump(gmp_init('0X', 16));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(gmp_init('0x', 16));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo 'Binary', \PHP_EOL;
try {
    var_dump(gmp_init('0B', 2));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(gmp_init('0b', 2));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo 'Fuzzing gmp functions:', \PHP_EOL;
foreach ($functions1 as $function) {
    try {
        $function('0B');
        echo $function, ' failed with 0B', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function('0b');
        echo $function, ' failed with 0b', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function('0X');
        echo $function, ' failed with 0X', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function('0x');
        echo $function, ' failed with 0x', \PHP_EOL;
    } catch (\ValueError) { }
}
foreach ($functions1_need_int_2 as $function) {
    try {
        $function('0B', 1);
        echo $function, ' failed with 0B', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function('0b', 1);
        echo $function, ' failed with 0b', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function('0X', 1);
        echo $function, ' failed with 0X', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function('0x', 1);
        echo $function, ' failed with 0x', \PHP_EOL;
    } catch (\ValueError) { }
}
foreach ($functions2 as $function) {
    try {
        $function('0B', 1);
        echo $function, ' arg 1 failed with 0B', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function('0b', 1);
        echo $function, ' arg 1 failed with 0b', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function('0X', 1);
        echo $function, ' arg 1 failed with 0X', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function('0x', 1);
        echo $function, ' arg 1 failed with 0x', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function(1, '0B');
        echo $function, ' arg 2 failed with 0B', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function(1, '0b');
        echo $function, ' arg 2 failed with 0b', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function(1, '0X');
        echo $function, ' arg 2 failed with 0X', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function(1, '0x');
        echo $function, ' arg 2 failed with 0x', \PHP_EOL;
    } catch (\ValueError) { }
}
foreach ($functions3 as $function) {
    try {
        $function('0B', 1, 1);
        echo $function, ' arg 1 failed with 0B', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function('0b', 1, 1);
        echo $function, ' arg 1 failed with 0b', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function('0X', 1, 1);
        echo $function, ' arg 1 failed with 0X', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function('0x', 1, 1);
        echo $function, ' arg 1 failed with 0x', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function(1, '0B', 1);
        echo $function, ' arg 2 failed with 0B', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function(1, '0b', 1);
        echo $function, ' arg 2 failed with 0b', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function(1, '0X', 1);
        echo $function, ' arg 2 failed with 0X', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function(1, '0x', 1);
        echo $function, ' arg 2 failed with 0x', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function(1, 1, '0B');
        echo $function, ' arg 3 failed with 0B', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function(1, 1, '0b');
        echo $function, ' arg 3 failed with 0b', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function(1, 1, '0X');
        echo $function, ' arg 3 failed with 0X', \PHP_EOL;
    } catch (\ValueError) { }
    try {
        $function(1, 1, '0x');
        echo $function, ' arg 3 failed with 0x', \PHP_EOL;
    } catch (\ValueError) { }
}

echo "Done\n";
?>
--EXPECT--
Explicit base with gmp_init:
Hexadecimal
gmp_init(): Argument #1 ($num) is not an integer string
gmp_init(): Argument #1 ($num) is not an integer string
Binary
gmp_init(): Argument #1 ($num) is not an integer string
gmp_init(): Argument #1 ($num) is not an integer string
Fuzzing gmp functions:
Done
