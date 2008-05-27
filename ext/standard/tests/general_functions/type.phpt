--TEST--
gettype(), settype() and friends
--FILE--
<?php

function foo($errno, $errstr, $errfile, $errline) {
	var_dump($errstr);
}

set_error_handler("foo");

$fp = fopen(__FILE__, "r");
fclose($fp);
$fp1 = fopen(__FILE__, "r");

$var1 = "another string";
$var2 = array(2,3,4);

$array = array(
	array(1,2,3),
	$var1,
	$var2,
	1,
	2.0,
	NULL,
	false,
	"some string",
	$fp,
	$fp1,
	new stdclass,
);

$types = array(
	"null",
	"integer",
	"double",
	"boolean",
	"resource",
	"array",
	"object",
	"string"
	);

foreach ($array as $var) {
	var_dump(gettype($var));
}

foreach ($types as $type) {
	foreach ($array as $var) {
		var_dump(settype($var, $type));
		var_dump($var);
	}
}

echo "Done\n";
?>
--EXPECTF--
unicode(5) "array"
unicode(7) "unicode"
unicode(5) "array"
unicode(7) "integer"
unicode(6) "double"
unicode(4) "NULL"
unicode(7) "boolean"
unicode(7) "unicode"
unicode(12) "unknown type"
unicode(8) "resource"
unicode(6) "object"
bool(true)
NULL
bool(true)
NULL
bool(true)
NULL
bool(true)
NULL
bool(true)
NULL
bool(true)
NULL
bool(true)
NULL
bool(true)
NULL
bool(true)
NULL
bool(true)
NULL
bool(true)
NULL
bool(true)
int(1)
bool(true)
int(0)
bool(true)
int(1)
bool(true)
int(1)
bool(true)
int(2)
bool(true)
int(0)
bool(true)
int(0)
bool(true)
int(0)
bool(true)
int(%d)
bool(true)
int(%d)
unicode(54) "Object of class stdClass could not be converted to int"
bool(true)
int(%d)
bool(true)
float(1)
bool(true)
float(0)
bool(true)
float(1)
bool(true)
float(1)
bool(true)
float(2)
bool(true)
float(0)
bool(true)
float(0)
bool(true)
float(0)
bool(true)
float(%d)
bool(true)
float(%d)
unicode(57) "Object of class stdClass could not be converted to double"
bool(true)
float(%d)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
unicode(42) "settype(): Cannot convert to resource type"
bool(false)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
unicode(42) "settype(): Cannot convert to resource type"
bool(false)
unicode(14) "another string"
unicode(42) "settype(): Cannot convert to resource type"
bool(false)
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
}
unicode(42) "settype(): Cannot convert to resource type"
bool(false)
int(1)
unicode(42) "settype(): Cannot convert to resource type"
bool(false)
float(2)
unicode(42) "settype(): Cannot convert to resource type"
bool(false)
NULL
unicode(42) "settype(): Cannot convert to resource type"
bool(false)
bool(false)
unicode(42) "settype(): Cannot convert to resource type"
bool(false)
unicode(11) "some string"
unicode(42) "settype(): Cannot convert to resource type"
bool(false)
resource(%d) of type (Unknown)
unicode(42) "settype(): Cannot convert to resource type"
bool(false)
resource(%d) of type (stream)
unicode(42) "settype(): Cannot convert to resource type"
bool(false)
object(stdClass)#%d (0) {
}
bool(true)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
bool(true)
array(1) {
  [0]=>
  unicode(14) "another string"
}
bool(true)
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
}
bool(true)
array(1) {
  [0]=>
  int(1)
}
bool(true)
array(1) {
  [0]=>
  float(2)
}
bool(true)
array(0) {
}
bool(true)
array(1) {
  [0]=>
  bool(false)
}
bool(true)
array(1) {
  [0]=>
  unicode(11) "some string"
}
bool(true)
array(1) {
  [0]=>
  resource(%d) of type (Unknown)
}
bool(true)
array(1) {
  [0]=>
  resource(%d) of type (stream)
}
bool(true)
array(0) {
}
bool(true)
object(stdClass)#%d (3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
bool(true)
object(stdClass)#%d (1) {
  [u"scalar"]=>
  unicode(14) "another string"
}
bool(true)
object(stdClass)#%d (3) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
}
bool(true)
object(stdClass)#%d (1) {
  [u"scalar"]=>
  int(1)
}
bool(true)
object(stdClass)#%d (1) {
  [u"scalar"]=>
  float(2)
}
bool(true)
object(stdClass)#%d (0) {
}
bool(true)
object(stdClass)#%d (1) {
  [u"scalar"]=>
  bool(false)
}
bool(true)
object(stdClass)#%d (1) {
  [u"scalar"]=>
  unicode(11) "some string"
}
bool(true)
object(stdClass)#%d (1) {
  [u"scalar"]=>
  resource(%d) of type (Unknown)
}
bool(true)
object(stdClass)#%d (1) {
  [u"scalar"]=>
  resource(%d) of type (stream)
}
bool(true)
object(stdClass)#%d (0) {
}
unicode(26) "Array to string conversion"
bool(true)
unicode(5) "Array"
bool(true)
unicode(14) "another string"
unicode(26) "Array to string conversion"
bool(true)
unicode(5) "Array"
bool(true)
unicode(1) "1"
bool(true)
unicode(1) "2"
bool(true)
unicode(0) ""
bool(true)
unicode(0) ""
bool(true)
unicode(11) "some string"
bool(true)
unicode(14) "Resource id #%d"
bool(true)
unicode(14) "Resource id #%d"
unicode(65) "Object of class stdClass could not be converted to Unicode string"
unicode(45) "Object of class stdClass to string conversion"
bool(true)
unicode(6) "Object"
Done
