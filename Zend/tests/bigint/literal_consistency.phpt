--TEST--
Bigint literal consistency
--DESCRIPTION--
This tests various integer values to ensure they are all integers, bigint or no
This test is designed to run on all platforms; it should produce the exact same result on all of them, as that is the goal of bigints
--FILE--
<?php

var_dump(0b1111111111111111111111111111111);    /* 32-bit max */
var_dump(-0b10000000000000000000000000000000);  /* 32-bit min */
var_dump(0b111111111111111111111111111111111111111111111111111111111111111);    /* 64-bit max */
var_dump(-0b1000000000000000000000000000000000000000000000000000000000000000);  /* 64-bit min */

var_dump(0b11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111);   /* 128-bit max */

echo PHP_EOL;

var_dump(017777777777);                         /* 32-bit max */
var_dump(-020000000000);                        /* 32-bit min */
var_dump(0777777777777777777777);               /* 64-bit max */
var_dump(-01000000000000000000000);             /* 64-bit min */

var_dump(03777777777777777777777777777777777777777777); /* 128-bit max */

echo PHP_EOL;

var_dump(2147483647);                           /* 32-bit max */
var_dump(-2147483648);                          /* 32-bit min */
var_dump(9223372036854775807);                  /* 64-bit max */
var_dump(-9223372036854775808);                 /* 64-bit min */

var_dump(340282366920938463463374607431768211455);  /* 128-bit max */

echo PHP_EOL;

var_dump(0x7FFFFFFF);                           /* 32-bit max */
var_dump(-0x80000000);                          /* 32-bit min */
var_dump(0x7FFFFFFFFFFFFFFF);                   /* 64-bit max */
var_dump(-0x8000000000000000);                  /* 64-bit min */

var_dump(0xffffffffffffffffffffffffffffffff);   /* 128-bit max */

--EXPECT--
int(2147483647)
int(-2147483648)
int(9223372036854775807)
int(-9223372036854775808)
int(340282366920938463463374607431768211455)

int(2147483647)
int(-2147483648)
int(9223372036854775807)
int(-9223372036854775808)
int(340282366920938463463374607431768211455)

int(2147483647)
int(-2147483648)
int(9223372036854775807)
int(-9223372036854775808)
int(340282366920938463463374607431768211455)

int(2147483647)
int(-2147483648)
int(9223372036854775807)
int(-9223372036854775808)
int(340282366920938463463374607431768211455)