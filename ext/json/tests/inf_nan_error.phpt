--TEST--
An error is thrown when INF or NaN are encoded
--FILE--
<?php

$inf = INF;

var_dump($inf);

var_dump(json_encode($inf));
var_dump(json_last_error(), json_last_error_msg());

var_dump(json_encode($inf, JSON_PARTIAL_OUTPUT_ON_ERROR));
var_dump(json_last_error(), json_last_error_msg());

echo "\n";

$nan = NAN;

var_dump($nan);

var_dump(json_encode($nan));
var_dump(json_last_error(), json_last_error_msg());

var_dump(json_encode($nan, JSON_PARTIAL_OUTPUT_ON_ERROR));
var_dump(json_last_error(), json_last_error_msg());
?>
--EXPECT--
float(INF)
bool(false)
int(7)
string(34) "Inf and NaN cannot be JSON encoded"
string(1) "0"
int(7)
string(34) "Inf and NaN cannot be JSON encoded"

float(NAN)
bool(false)
int(7)
string(34) "Inf and NaN cannot be JSON encoded"
string(1) "0"
int(7)
string(34) "Inf and NaN cannot be JSON encoded"
