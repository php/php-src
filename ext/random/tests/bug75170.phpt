--TEST--
Bug #75170: mt_rand() bias on 64-bit machines
--CREDITS--
Solar Designer in https://externals.io/message/100229
--FILE--
<?php

// PHP pre-7.1.0 modulo bias
mt_srand(1234567890);
$total = 10000;
$max = 0x66666666;
$halves[0] = $halves[1] = 0;
for ($i = 0; $i < $total; $i++) {
    $halves[(mt_rand(0, $max - 1) >> 1) & 1]++;
}
printf("%.1f%% vs. %.1f%%\n", 100. * $halves[0] / $total, 100. * $halves[1] / $total);

// PHP 7.1.0 to 7.2.0beta2 modulo bias bug found during work
// on http://www.openwall.com/php_mt_seed/
mt_srand(1234567890);
$total = 10000;
$max = 0x66666666;
$halves[0] = $halves[1] = 0;
for ($i = 0; $i < $total; $i++) {
    $halves[(int) (mt_rand(0, $max - 1) / ($max / 2))]++;
}
printf("%.1f%% vs. %.1f%%\n", 100. * $halves[0] / $total, 100. * $halves[1] / $total);

?>
--EXPECT--
49.5% vs. 50.5%
50.5% vs. 49.5%
