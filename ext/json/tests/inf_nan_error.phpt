--TEST--
An error is thrown when INF or NaN are encoded
--FILE--
<?php

$inf = INF;

var_dump($inf);

var_dump(json_encode($inf));
var_dump(json_last_error());

var_dump(json_encode($inf, JSON_PARTIAL_OUTPUT_ON_ERROR));
var_dump(json_last_error());

$nan = NAN;

var_dump($nan);

var_dump(json_encode($nan));
var_dump(json_last_error());

var_dump(json_encode($nan, JSON_PARTIAL_OUTPUT_ON_ERROR));
var_dump(json_last_error());
?>
--EXPECTF--
float(INF)

Warning: json_encode(): double INF does not conform to the JSON spec in %s on line %d
bool(false)
int(7)

Warning: json_encode(): double INF does not conform to the JSON spec in %s on line %d
string(1) "0"
int(7)
float(NAN)

Warning: json_encode(): double NAN does not conform to the JSON spec in %s on line %d
bool(false)
int(7)

Warning: json_encode(): double NAN does not conform to the JSON spec in %s on line %d
string(1) "0"
int(7)
