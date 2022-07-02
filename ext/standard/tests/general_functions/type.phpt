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
        try {
            var_dump(settype($var, $type));
        } catch (Error $e) {
            echo "Error: ", $e->getMessage(), "\n";
        }
        var_dump($var);
    }
}

echo "Done\n";
?>
--EXPECTF--
string(5) "array"
string(6) "string"
string(5) "array"
string(7) "integer"
string(6) "double"
string(4) "NULL"
string(7) "boolean"
string(6) "string"
string(17) "resource (closed)"
string(8) "resource"
string(6) "object"
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
string(54) "Object of class stdClass could not be converted to int"
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
float(%f)
bool(true)
float(%f)
string(56) "Object of class stdClass could not be converted to float"
bool(true)
float(%f)
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
Error: Cannot convert to resource type
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
Error: Cannot convert to resource type
string(14) "another string"
Error: Cannot convert to resource type
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
}
Error: Cannot convert to resource type
int(1)
Error: Cannot convert to resource type
float(2)
Error: Cannot convert to resource type
NULL
Error: Cannot convert to resource type
bool(false)
Error: Cannot convert to resource type
string(11) "some string"
Error: Cannot convert to resource type
resource(%d) of type (Unknown)
Error: Cannot convert to resource type
resource(%d) of type (stream)
Error: Cannot convert to resource type
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
  string(14) "another string"
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
  string(11) "some string"
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
  ["0"]=>
  int(1)
  ["1"]=>
  int(2)
  ["2"]=>
  int(3)
}
bool(true)
object(stdClass)#%d (1) {
  ["scalar"]=>
  string(14) "another string"
}
bool(true)
object(stdClass)#%d (3) {
  ["0"]=>
  int(2)
  ["1"]=>
  int(3)
  ["2"]=>
  int(4)
}
bool(true)
object(stdClass)#%d (1) {
  ["scalar"]=>
  int(1)
}
bool(true)
object(stdClass)#%d (1) {
  ["scalar"]=>
  float(2)
}
bool(true)
object(stdClass)#%d (0) {
}
bool(true)
object(stdClass)#%d (1) {
  ["scalar"]=>
  bool(false)
}
bool(true)
object(stdClass)#%d (1) {
  ["scalar"]=>
  string(11) "some string"
}
bool(true)
object(stdClass)#%d (1) {
  ["scalar"]=>
  resource(%d) of type (Unknown)
}
bool(true)
object(stdClass)#%d (1) {
  ["scalar"]=>
  resource(%d) of type (stream)
}
bool(true)
object(stdClass)#%d (0) {
}
string(26) "Array to string conversion"
bool(true)
string(5) "Array"
bool(true)
string(14) "another string"
string(26) "Array to string conversion"
bool(true)
string(5) "Array"
bool(true)
string(1) "1"
bool(true)
string(1) "2"
bool(true)
string(0) ""
bool(true)
string(0) ""
bool(true)
string(11) "some string"
bool(true)
string(14) "Resource id #%d"
bool(true)
string(14) "Resource id #%d"
Error: Object of class stdClass could not be converted to string
string(0) ""
Done
