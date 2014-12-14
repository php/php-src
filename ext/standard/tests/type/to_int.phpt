--TEST--
to_int()
--FILE--
<?php

function to_int_wrapper($value) {
    try {
        return to_int($value);
    } catch (CastException $e) {
        return NULL;
    }
}

// should pass
var_dump(to_int_wrapper("0"));
var_dump(to_int_wrapper(0));
var_dump(to_int_wrapper(0.0));
var_dump(to_int_wrapper("10"));
var_dump(to_int_wrapper("+10"));
var_dump(to_int_wrapper("-10"));
var_dump(to_int_wrapper(10));
var_dump(to_int_wrapper(10.0));
var_dump(to_int_wrapper((string)PHP_INT_MAX));
var_dump(to_int_wrapper(PHP_INT_MAX));
var_dump(to_int_wrapper((string)PHP_INT_MIN));
var_dump(to_int_wrapper(PHP_INT_MIN));

echo PHP_EOL;

// shouldn't pass
var_dump(to_int_wrapper(""));
var_dump(to_int_wrapper("10.0"));
var_dump(to_int_wrapper("75e-5"));
var_dump(to_int_wrapper("31e+7"));
var_dump(to_int_wrapper("0x10"));

echo PHP_EOL;

// disallowed types
var_dump(to_int_wrapper(NULL));
var_dump(to_int_wrapper(TRUE));
var_dump(to_int_wrapper(FALSE));
var_dump(to_int_wrapper(new StdClass));
var_dump(to_int_wrapper(fopen("data:text/html,foobar", "r")));
var_dump(to_int_wrapper([]));

echo PHP_EOL;

// check truncation
var_dump(to_int_wrapper(1.5));
var_dump(to_int_wrapper("1.5"));

echo PHP_EOL;

// reject leading, trailing chars
var_dump(to_int_wrapper("010"));
var_dump(to_int_wrapper("10abc"));
var_dump(to_int_wrapper("abc10"));
var_dump(to_int_wrapper("   100    "));
var_dump(to_int_wrapper("\n\t\v\r\f   78 \n \t\v\r\f   \n"));
var_dump(to_int_wrapper("\n\t\v\r\f78"));
var_dump(to_int_wrapper("78\n\t\v\r\f"));

echo PHP_EOL;

// overflow/nan/inf
var_dump(to_int_wrapper(INF));
var_dump(to_int_wrapper(-INF));
var_dump(to_int_wrapper(NAN));
var_dump(to_int_wrapper(PHP_INT_MAX * 2));
var_dump(to_int_wrapper(PHP_INT_MIN * 2));
var_dump(to_int_wrapper((string)(PHP_INT_MAX * 2)));
var_dump(to_int_wrapper((string)(PHP_INT_MIN * 2)));
--EXPECTF--
int(0)
int(0)
int(0)
int(10)
int(10)
int(-10)
int(10)
int(10)
int(%d)
int(%d)
int(-%d)
int(-%d)

NULL
NULL
NULL
NULL
NULL

NULL
NULL
NULL
NULL
NULL
NULL

NULL
NULL

NULL
NULL
NULL
NULL
NULL
NULL
NULL

NULL
NULL
NULL
NULL
NULL
NULL
NULL
