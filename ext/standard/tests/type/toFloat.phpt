--TEST--
toFloat()
--FILE--
<?php

// should pass
var_dump(toFloat("0"));
var_dump(toFloat(0));
var_dump(toFloat(0.0));
var_dump(toFloat("10"));
var_dump(toFloat(10));
var_dump(toFloat(10.0));
var_dump(toFloat((string)PHP_INT_MAX));
var_dump(toFloat(PHP_INT_MAX));
var_dump(toFloat((float)PHP_INT_MAX));
var_dump(toFloat((string)PHP_INT_MIN));
var_dump(toFloat(PHP_INT_MIN));
var_dump(toFloat((float)PHP_INT_MIN));

echo PHP_EOL;

// disallowed types
var_dump(toFloat(NULL));
var_dump(toFloat(TRUE));
var_dump(toFloat(FALSE));
var_dump(toFloat(new StdClass));
var_dump(toFloat(fopen("data:text/html,foobar", "r")));
var_dump(toFloat([]));

echo PHP_EOL;

var_dump(toFloat(1.5));
var_dump(toFloat("1.5"));

echo PHP_EOL;

// reject leading, trailing chars
var_dump(toFloat("10abc"));
var_dump(toFloat("abc10"));

echo PHP_EOL;

// accept leading, trailing whitespace
var_dump(toFloat("   100    "));
var_dump(toFloat("\n\t\v\r\f   78 \n \t\v\r\f   \n"));
var_dump(toFloat("\n\t\v\r\f78"));
var_dump(toFloat("78\n\t\v\r\f"));

echo PHP_EOL;

// overflow/nan/inf
var_dump(toFloat(INF));
var_dump(toFloat(-INF));
var_dump(toFloat(NAN));
var_dump(toFloat(PHP_INT_MAX * 2));
var_dump(toFloat(PHP_INT_MIN * 2));

echo PHP_EOL;

// check exponents work
var_dump(toFloat("75e-5"));
var_dump(toFloat("31e+7"));
--EXPECTF--
float(0)
float(0)
float(0)
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

float(1.5)
float(1.5)

NULL
NULL

float(100)
float(78)
float(78)
float(78)

float(INF)
float(-INF)
float(NAN)
float(%s)
float(%s)

float(0.00075)
float(310000000)
