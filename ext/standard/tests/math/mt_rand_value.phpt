--TEST--
Test mt_rand() output
--FILE--
<?php

mt_srand(12345678);

for ($i=0; $i<16; $i++) {
    echo mt_rand().PHP_EOL;
}
echo PHP_EOL;

$x = 0;
for ($i=0; $i<1024; $i++) {
    $x ^= mt_rand();
}
echo $x.PHP_EOL;

/*
 * Note that the output will be different from the original mt19937ar.c,
 * because PHP's implementation contains a bug. Thus, this test actually
 * checks to make sure that PHP's behaviour is wrong, but consistently so.
 */

?>
--EXPECTF--
1614640687
1711027313
857485497
688176834
1386682158
412773096
813703253
898651287
2087374214
1382556330
1640700129
1863374167
1324097651
1923803667
676334965
853386222

1571178311
