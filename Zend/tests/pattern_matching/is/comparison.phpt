--TEST--
Comparison pattern
--FILE--
<?php

echo "Basic comparison\n";

var_dump(0 is <0);
var_dump(0 is <1);
var_dump(1 is <0);
var_dump(0 is <=0);
var_dump(0 is <=1);
var_dump(1 is <=0);
var_dump(0 is >0);
var_dump(0 is >1);
var_dump(1 is >0);
var_dump(0 is >=0);
var_dump(0 is >=1);
var_dump(1 is >=0);

echo "\nNumeric strings\n";

var_dump('1' is >0);
var_dump('1e2' is >99);
var_dump('1e2' is >101);
var_dump('3.141' is >3);
var_dump('3.141' is >4);

echo "\nNon-numeric\n";

var_dump('foo' is >0);
var_dump('10foo' is >0);
var_dump('10 foo' is >0);
var_dump([] is >0);
var_dump(new stdClass is >0);

?>
--EXPECT--
Basic comparison
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)

Numeric strings
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)

Non-numeric
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
