--TEST--
bcmul() checking overflow
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
for ($i = 1; $i < 15; $i++) {
    $repeat = 2 ** $i;
    $num1 = str_repeat('99999999', $repeat);
    /*
     * 9999 * 9999 = 99980001
     * 99999999 * 99999999 = 9999999800000001
     */
    $expected = str_repeat('9', $repeat * 8 - 1) . '8' . str_repeat('0', $repeat * 8 - 1) . '1';
    $actual = bcmul($num1, $num1);
    echo $repeat . ': ' . ($actual === $expected ? 'OK' : 'NG') . PHP_EOL;
}
?>
--EXPECT--
2: OK
4: OK
8: OK
16: OK
32: OK
64: OK
128: OK
256: OK
512: OK
1024: OK
2048: OK
4096: OK
8192: OK
16384: OK
