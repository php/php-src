--TEST--
Simple filter_var() tests
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--INI--
precision=14
--FILE--
<?php

/* Integer */
$data = "-123";   var_dump(filter_var($data, FILTER_VALIDATE_INT));
$data = "0";      var_dump(filter_var($data, FILTER_VALIDATE_INT));
$data = "-0";     var_dump(filter_var($data, FILTER_VALIDATE_INT));
$data = "+0";     var_dump(filter_var($data, FILTER_VALIDATE_INT));
$data = "123";    var_dump(filter_var($data, FILTER_VALIDATE_INT));
$data = -123;     var_dump(filter_var($data, FILTER_VALIDATE_INT));
$data = 0;        var_dump(filter_var($data, FILTER_VALIDATE_INT));
$data = -0;        var_dump(filter_var($data, FILTER_VALIDATE_INT));
$data = +0;        var_dump(filter_var($data, FILTER_VALIDATE_INT));
$data = 123;      var_dump(filter_var($data, FILTER_VALIDATE_INT));
$data = "";       var_dump(filter_var($data, FILTER_VALIDATE_INT));
echo "\n";

/* Float */
$data = "-0.123"; var_dump(filter_var($data, FILTER_VALIDATE_FLOAT));
$data = "0.00";   var_dump(filter_var($data, FILTER_VALIDATE_FLOAT));
$data = "1.23";   var_dump(filter_var($data, FILTER_VALIDATE_FLOAT));
$data = -1.23;    var_dump(filter_var($data, FILTER_VALIDATE_FLOAT));
$data = 0.0;      var_dump(filter_var($data, FILTER_VALIDATE_FLOAT));
$data = 1.23;     var_dump(filter_var($data, FILTER_VALIDATE_FLOAT));
$data = "";       var_dump(filter_var($data, FILTER_VALIDATE_FLOAT));
echo "\n";

/* Boolean */
$data = "on";     var_dump(filter_var($data, FILTER_VALIDATE_BOOLEAN));
$data = "off";    var_dump(filter_var($data, FILTER_VALIDATE_BOOLEAN));
$data = "yes";    var_dump(filter_var($data, FILTER_VALIDATE_BOOLEAN));
$data = "no";     var_dump(filter_var($data, FILTER_VALIDATE_BOOLEAN));
$data = "true";   var_dump(filter_var($data, FILTER_VALIDATE_BOOLEAN));
$data = "false";  var_dump(filter_var($data, FILTER_VALIDATE_BOOLEAN));
$data = "1";      var_dump(filter_var($data, FILTER_VALIDATE_BOOLEAN));
$data = "0";      var_dump(filter_var($data, FILTER_VALIDATE_BOOLEAN));
$data = 1;        var_dump(filter_var($data, FILTER_VALIDATE_BOOLEAN));
$data = 0;        var_dump(filter_var($data, FILTER_VALIDATE_BOOLEAN));
$data = true;     var_dump(filter_var($data, FILTER_VALIDATE_BOOLEAN));
$data = false;    var_dump(filter_var($data, FILTER_VALIDATE_BOOLEAN));
$data = "";       var_dump(filter_var($data, FILTER_VALIDATE_BOOLEAN));

?>
--EXPECT--
int(-123)
int(0)
int(0)
int(0)
int(123)
int(-123)
int(0)
int(0)
int(0)
int(123)
bool(false)

float(-0.123)
float(0)
float(1.23)
float(-1.23)
float(0)
float(1.23)
bool(false)

bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
