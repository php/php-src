--TEST--
try_float()
--FILE--
<?php

// should pass
var_dump(try_float("0"));
var_dump(try_float(0));
var_dump(try_float(0.0));
var_dump(try_float("10"));
var_dump(try_float("+10"));
var_dump(try_float("-10"));
var_dump(try_float("10.0"));
var_dump(try_float(10));
var_dump(try_float(10.0));
var_dump(try_float((string)PHP_INT_MAX));
var_dump(try_float(PHP_INT_MAX));
var_dump(try_float((float)PHP_INT_MAX));
var_dump(try_float((string)PHP_INT_MIN));
var_dump(try_float(PHP_INT_MIN));
var_dump(try_float((float)PHP_INT_MIN));

echo PHP_EOL;

// disallowed types
var_dump(try_float(""));
var_dump(try_float("0x10"));
var_dump(try_float(NULL));
var_dump(try_float(TRUE));
var_dump(try_float(FALSE));
var_dump(try_float(new StdClass));
var_dump(try_float(fopen("data:text/html,foobar", "r")));
var_dump(try_float([]));

echo PHP_EOL;

var_dump(try_float(1.5));
var_dump(try_float("1.5"));

echo PHP_EOL;

// reject leading, trailing chars
var_dump(try_float("010"));
var_dump(try_float("10abc"));
var_dump(try_float("abc10"));
var_dump(try_float("   100    "));
var_dump(try_float("\n\t\v\r\f   78 \n \t\v\r\f   \n"));
var_dump(try_float("\n\t\v\r\f78"));
var_dump(try_float("78\n\t\v\r\f"));

echo PHP_EOL;

// overflow/nan/inf
var_dump(try_float(INF));
var_dump(try_float(-INF));
var_dump(try_float(NAN));
var_dump(try_float(PHP_INT_MAX * 2));
var_dump(try_float(PHP_INT_MIN * 2));
var_dump(try_float((string)(PHP_INT_MAX * 2)));
var_dump(try_float((string)(PHP_INT_MIN * 2)));

echo PHP_EOL;

// check exponents work
var_dump(try_float("75e-5"));
var_dump(try_float("31e+7"));
--EXPECTF--
float(0)
float(0)
float(0)
float(10)
float(10)
float(-10)
float(10)
float(10)
float(10)
float(%s)
float(%s)
float(%s)
float(-%s)
float(-%s)
float(%s)

NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL

float(1.5)
float(1.5)

NULL
NULL
NULL
NULL
NULL
NULL
NULL

float(INF)
float(-INF)
float(NAN)
float(%s)
float(%s)
float(%s)
float(%s)

float(0.00075)
float(310000000)
