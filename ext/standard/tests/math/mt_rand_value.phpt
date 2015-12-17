--TEST--
Test mt_rand() - returns the exact same values as mt19937ar.c
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
excpect values are obtained from original mt19937ar.c as follows:

int i, x;
init_genrand(12345678);
for (i=0; i<16; i++) {
    printf("%d\n", genrand_int31());
}
printf("\n");
x = 0;
for (i=0; i<1024; i++) {
    x ^= genrand_int31();
}
printf("%d\n", x);
*/
?>
--EXPECTF--
527860569
1711027313
1280820687
688176834
770499160
412773096
813703253
898651287
52508912
757323740
511765911
274407457
833082629
1923803667
1461450755
1301698200

1612214270
