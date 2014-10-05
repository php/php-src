--TEST--
toInt()
--FILE--
<?php

// should pass
var_dump(toInt("0"));
var_dump(toInt(0));
var_dump(toInt(0.0));
var_dump(toInt("10"));
var_dump(toInt(10));
var_dump(toInt(10.0));
var_dump(toInt((string)PHP_INT_MAX));
var_dump(toInt(PHP_INT_MAX));
var_dump(toInt((string)PHP_INT_MIN));
var_dump(toInt(PHP_INT_MIN));

echo PHP_EOL;

// disallowed types
var_dump(toInt(NULL));
var_dump(toInt(TRUE));
var_dump(toInt(FALSE));
var_dump(toInt(new StdClass));
var_dump(toInt(fopen("data:text/html,foobar", "r")));
var_dump(toInt([]));

echo PHP_EOL;

// bases
var_dump(toInt("010", 0)); // base detect octal
var_dump(toInt("010", 10)); // not octal
var_dump(toInt("010"));     // not octal
var_dump(toInt("0x10", 0)); // base detect hex
var_dump(toInt("0x10", 10)); // not hex
var_dump(toInt("0x10"));    // not hex
var_dump(toInt("123ayz", 36));
var_dump(toInt("123AyZ", 36));

echo PHP_EOL;

// check truncation
var_dump(toInt(1.5));
var_dump(toInt("1.5"));

echo PHP_EOL;

// reject leading, trailing chars
var_dump(toInt("10abc"));
var_dump(toInt("123abcxyz", 13));
var_dump(toInt("abc10"));
var_dump(toInt("abcxyz123", 13));

echo PHP_EOL;

// accept leading, trailing whitespace
var_dump(toInt("   100    "));
var_dump(toInt("\n\t\v\r\f   78 \n \t\v\r\f   \n"));
var_dump(toInt("\n\t\v\r\f78"));
var_dump(toInt("78\n\t\v\r\f"));

echo PHP_EOL;

// overflow/nan/inf
var_dump(toInt(INF));
var_dump(toInt(-INF));
var_dump(toInt(NAN));
var_dump(toInt(PHP_INT_MAX * 2));
var_dump(toInt(PHP_INT_MIN * 2));
--EXPECTF--
int(0)
int(0)
int(0)
int(10)
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

int(8)
int(10)
int(10)
int(16)
NULL
NULL
int(63979595)
int(63979595)

int(1)
NULL

NULL
NULL
NULL
NULL

int(100)
int(78)
int(78)
int(78)

NULL
NULL
NULL
NULL
NULL
