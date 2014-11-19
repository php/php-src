--TEST--
try_int()
--FILE--
<?php

// should pass
var_dump(try_int("0"));
var_dump(try_int(0));
var_dump(try_int(0.0));
var_dump(try_int("10"));
var_dump(try_int("+10"));
var_dump(try_int("-10"));
var_dump(try_int(10));
var_dump(try_int(10.0));
var_dump(try_int((string)PHP_INT_MAX));
var_dump(try_int(PHP_INT_MAX));
var_dump(try_int((string)PHP_INT_MIN));
var_dump(try_int(PHP_INT_MIN));

echo PHP_EOL;

// shouldn't pass
var_dump(try_int(""));
var_dump(try_int("10.0"));
var_dump(try_int("75e-5"));
var_dump(try_int("31e+7"));
var_dump(try_int("0x10"));

echo PHP_EOL;

// disallowed types
var_dump(try_int(NULL));
var_dump(try_int(TRUE));
var_dump(try_int(FALSE));
var_dump(try_int(new StdClass));
var_dump(try_int(fopen("data:text/html,foobar", "r")));
var_dump(try_int([]));

echo PHP_EOL;

// check truncation
var_dump(try_int(1.5));
var_dump(try_int("1.5"));

echo PHP_EOL;

// reject leading, trailing chars
var_dump(try_int("010"));
var_dump(try_int("10abc"));
var_dump(try_int("abc10"));
var_dump(try_int("   100    "));
var_dump(try_int("\n\t\v\r\f   78 \n \t\v\r\f   \n"));
var_dump(try_int("\n\t\v\r\f78"));
var_dump(try_int("78\n\t\v\r\f"));

echo PHP_EOL;

// overflow/nan/inf
var_dump(try_int(INF));
var_dump(try_int(-INF));
var_dump(try_int(NAN));
var_dump(try_int(PHP_INT_MAX * 2));
var_dump(try_int(PHP_INT_MIN * 2));
var_dump(try_int((string)(PHP_INT_MAX * 2)));
var_dump(try_int((string)(PHP_INT_MIN * 2)));
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
