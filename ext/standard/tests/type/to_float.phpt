--TEST--
to_float()
--FILE--
<?php

function to_float_wrapper($value) {
    try {
        return to_float($value);
    } catch (CastException $e) {
        return NULL;
    }
}

// should pass
var_dump(to_float_wrapper("0"));
var_dump(to_float_wrapper(0));
var_dump(to_float_wrapper(0.0));
var_dump(to_float_wrapper("10"));
var_dump(to_float_wrapper("+10"));
var_dump(to_float_wrapper("-10"));
var_dump(to_float_wrapper("10.0"));
var_dump(to_float_wrapper(10));
var_dump(to_float_wrapper(10.0));
var_dump(to_float_wrapper((string)PHP_INT_MAX));
var_dump(to_float_wrapper(PHP_INT_MAX));
var_dump(to_float_wrapper((float)PHP_INT_MAX));
var_dump(to_float_wrapper((string)PHP_INT_MIN));
var_dump(to_float_wrapper(PHP_INT_MIN));
var_dump(to_float_wrapper((float)PHP_INT_MIN));

echo PHP_EOL;

// disallowed types
var_dump(to_float_wrapper(""));
var_dump(to_float_wrapper("0x10"));
var_dump(to_float_wrapper(NULL));
var_dump(to_float_wrapper(TRUE));
var_dump(to_float_wrapper(FALSE));
var_dump(to_float_wrapper(new StdClass));
var_dump(to_float_wrapper(fopen("data:text/html,foobar", "r")));
var_dump(to_float_wrapper([]));

echo PHP_EOL;

var_dump(to_float_wrapper(1.5));
var_dump(to_float_wrapper("1.5"));

echo PHP_EOL;

// reject leading, trailing chars
var_dump(to_float_wrapper("010"));
var_dump(to_float_wrapper("10abc"));
var_dump(to_float_wrapper("abc10"));
var_dump(to_float_wrapper("   100    "));
var_dump(to_float_wrapper("\n\t\v\r\f   78 \n \t\v\r\f   \n"));
var_dump(to_float_wrapper("\n\t\v\r\f78"));
var_dump(to_float_wrapper("78\n\t\v\r\f"));

echo PHP_EOL;

// overflow/nan/inf
var_dump(to_float_wrapper(INF));
var_dump(to_float_wrapper(-INF));
var_dump(to_float_wrapper(NAN));
var_dump(to_float_wrapper(PHP_INT_MAX * 2));
var_dump(to_float_wrapper(PHP_INT_MIN * 2));
var_dump(to_float_wrapper((string)(PHP_INT_MAX * 2)));
var_dump(to_float_wrapper((string)(PHP_INT_MIN * 2)));

echo PHP_EOL;

// check exponents work
var_dump(to_float_wrapper("75e-5"));
var_dump(to_float_wrapper("31e+7"));
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
