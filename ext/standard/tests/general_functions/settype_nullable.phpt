--TEST--
Test settype() function with nullable types
--INI--
precision=14
--FILE--
<?php

/* adapted from gettype_settype_basic.phpt */

function foo($errno, $errstr, $errfile, $errline) {
   echo "$errno: $errstr\n";
}

set_error_handler("foo");

echo "**** Testing settype() function with nullable types ****\n";

$fp = fopen(__FILE__, "r");
$dfp = opendir( dirname(__FILE__) );

$var1 = "another string";
$var2 = array(2,3,4);

class point
{
  var $x;
  var $y;

  function __construct($x, $y) {
     $this->x = $x;
     $this->y = $y;
  }

  function __toString() {
     return "Object";
  }
}

$values = array(
  array(1,2,3),
  $var1,
  $var2,
  1,
  -20,
  2.54,
  -2.54,
  NULL,
  false,
  "some string",
  'string',
  $fp,
  $dfp,
  new point(10,20)
);

$types = array(
  "null",
  "integer",
  "int",
  "float",
  "double",
  "boolean",
  "bool",
  "resource",
  "array",
  "object",
  "string"
);

foreach ($types as $type) {
  $nullable_type = "?$type";
  echo "\n-- Setting type of data to $nullable_type --\n";
  $loop_count = 1;
  foreach ($values as $var) {
     echo "-- Iteration $loop_count --\n"; $loop_count ++;
     // set to new type
     var_dump( settype($var, $nullable_type) );

     // dump the var
     var_dump( $var );

     // check the new type
     var_dump( gettype($var) );
  }

  var_dump( settype(${"undefined_var_$type"}, $nullable_type) );
  var_dump( ${"undefined_var_$type"} );
  var_dump( gettype(${"undefined_var_$type"}) );

  $unset_var = 10;
  unset($unset_var);
  var_dump( settype($unset_var, $nullable_type) );
  var_dump( $unset_var );
  var_dump( gettype($unset_var) );

  $ref_to_new_var = &${"new_var_$type"};
  $ref_to_ref_to_new_var = &$ref_to_new_var;
  var_dump( settype($ref_to_ref_to_new_var, $nullable_type) );
  var_dump( $ref_to_ref_to_new_var );
  var_dump( ${"new_var_$type"} );
  var_dump( gettype($ref_to_ref_to_new_var) );
  var_dump( gettype(${"new_var_$type"}) );
}

echo "Done\n";
?>
--EXPECTF--
**** Testing settype() function with nullable types ****

-- Setting type of data to ?null --
-- Iteration 1 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
-- Iteration 2 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
-- Iteration 3 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
-- Iteration 4 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
-- Iteration 5 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
-- Iteration 6 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
-- Iteration 7 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
-- Iteration 8 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
-- Iteration 9 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
-- Iteration 10 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
-- Iteration 11 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
-- Iteration 12 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
-- Iteration 13 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
-- Iteration 14 --
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
string(4) "NULL"
8: settype(): Redundant nullable prefix on null type
bool(true)
NULL
NULL
string(4) "NULL"
string(4) "NULL"

-- Setting type of data to ?integer --
-- Iteration 1 --
bool(true)
int(1)
string(7) "integer"
-- Iteration 2 --
bool(true)
int(0)
string(7) "integer"
-- Iteration 3 --
bool(true)
int(1)
string(7) "integer"
-- Iteration 4 --
bool(true)
int(1)
string(7) "integer"
-- Iteration 5 --
bool(true)
int(-20)
string(7) "integer"
-- Iteration 6 --
bool(true)
int(2)
string(7) "integer"
-- Iteration 7 --
bool(true)
int(-2)
string(7) "integer"
-- Iteration 8 --
bool(true)
NULL
string(4) "NULL"
-- Iteration 9 --
bool(true)
int(0)
string(7) "integer"
-- Iteration 10 --
bool(true)
int(0)
string(7) "integer"
-- Iteration 11 --
bool(true)
int(0)
string(7) "integer"
-- Iteration 12 --
bool(true)
int(%d)
string(7) "integer"
-- Iteration 13 --
bool(true)
int(%d)
string(7) "integer"
-- Iteration 14 --
8: Object of class point could not be converted to int
bool(true)
int(1)
string(7) "integer"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
NULL
string(4) "NULL"
string(4) "NULL"

-- Setting type of data to ?int --
-- Iteration 1 --
bool(true)
int(1)
string(7) "integer"
-- Iteration 2 --
bool(true)
int(0)
string(7) "integer"
-- Iteration 3 --
bool(true)
int(1)
string(7) "integer"
-- Iteration 4 --
bool(true)
int(1)
string(7) "integer"
-- Iteration 5 --
bool(true)
int(-20)
string(7) "integer"
-- Iteration 6 --
bool(true)
int(2)
string(7) "integer"
-- Iteration 7 --
bool(true)
int(-2)
string(7) "integer"
-- Iteration 8 --
bool(true)
NULL
string(4) "NULL"
-- Iteration 9 --
bool(true)
int(0)
string(7) "integer"
-- Iteration 10 --
bool(true)
int(0)
string(7) "integer"
-- Iteration 11 --
bool(true)
int(0)
string(7) "integer"
-- Iteration 12 --
bool(true)
int(%d)
string(7) "integer"
-- Iteration 13 --
bool(true)
int(%d)
string(7) "integer"
-- Iteration 14 --
8: Object of class point could not be converted to int
bool(true)
int(1)
string(7) "integer"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
NULL
string(4) "NULL"
string(4) "NULL"

-- Setting type of data to ?float --
-- Iteration 1 --
bool(true)
float(1)
string(6) "double"
-- Iteration 2 --
bool(true)
float(0)
string(6) "double"
-- Iteration 3 --
bool(true)
float(1)
string(6) "double"
-- Iteration 4 --
bool(true)
float(1)
string(6) "double"
-- Iteration 5 --
bool(true)
float(-20)
string(6) "double"
-- Iteration 6 --
bool(true)
float(2.54)
string(6) "double"
-- Iteration 7 --
bool(true)
float(-2.54)
string(6) "double"
-- Iteration 8 --
bool(true)
NULL
string(4) "NULL"
-- Iteration 9 --
bool(true)
float(0)
string(6) "double"
-- Iteration 10 --
bool(true)
float(0)
string(6) "double"
-- Iteration 11 --
bool(true)
float(0)
string(6) "double"
-- Iteration 12 --
bool(true)
float(%d)
string(6) "double"
-- Iteration 13 --
bool(true)
float(%d)
string(6) "double"
-- Iteration 14 --
8: Object of class point could not be converted to float
bool(true)
float(1)
string(6) "double"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
NULL
string(4) "NULL"
string(4) "NULL"

-- Setting type of data to ?double --
-- Iteration 1 --
bool(true)
float(1)
string(6) "double"
-- Iteration 2 --
bool(true)
float(0)
string(6) "double"
-- Iteration 3 --
bool(true)
float(1)
string(6) "double"
-- Iteration 4 --
bool(true)
float(1)
string(6) "double"
-- Iteration 5 --
bool(true)
float(-20)
string(6) "double"
-- Iteration 6 --
bool(true)
float(2.54)
string(6) "double"
-- Iteration 7 --
bool(true)
float(-2.54)
string(6) "double"
-- Iteration 8 --
bool(true)
NULL
string(4) "NULL"
-- Iteration 9 --
bool(true)
float(0)
string(6) "double"
-- Iteration 10 --
bool(true)
float(0)
string(6) "double"
-- Iteration 11 --
bool(true)
float(0)
string(6) "double"
-- Iteration 12 --
bool(true)
float(%d)
string(6) "double"
-- Iteration 13 --
bool(true)
float(%d)
string(6) "double"
-- Iteration 14 --
8: Object of class point could not be converted to float
bool(true)
float(1)
string(6) "double"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
NULL
string(4) "NULL"
string(4) "NULL"

-- Setting type of data to ?boolean --
-- Iteration 1 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 2 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 3 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 4 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 5 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 6 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 7 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 8 --
bool(true)
NULL
string(4) "NULL"
-- Iteration 9 --
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 10 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 11 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 12 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 13 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 14 --
bool(true)
bool(true)
string(7) "boolean"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
NULL
string(4) "NULL"
string(4) "NULL"

-- Setting type of data to ?bool --
-- Iteration 1 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 2 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 3 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 4 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 5 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 6 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 7 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 8 --
bool(true)
NULL
string(4) "NULL"
-- Iteration 9 --
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 10 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 11 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 12 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 13 --
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 14 --
bool(true)
bool(true)
string(7) "boolean"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
NULL
string(4) "NULL"
string(4) "NULL"

-- Setting type of data to ?resource --
-- Iteration 1 --
2: settype(): Cannot convert to resource type
bool(false)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
string(5) "array"
-- Iteration 2 --
2: settype(): Cannot convert to resource type
bool(false)
string(14) "another string"
string(6) "string"
-- Iteration 3 --
2: settype(): Cannot convert to resource type
bool(false)
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
}
string(5) "array"
-- Iteration 4 --
2: settype(): Cannot convert to resource type
bool(false)
int(1)
string(7) "integer"
-- Iteration 5 --
2: settype(): Cannot convert to resource type
bool(false)
int(-20)
string(7) "integer"
-- Iteration 6 --
2: settype(): Cannot convert to resource type
bool(false)
float(2.54)
string(6) "double"
-- Iteration 7 --
2: settype(): Cannot convert to resource type
bool(false)
float(-2.54)
string(6) "double"
-- Iteration 8 --
2: settype(): Cannot convert to resource type
bool(false)
NULL
string(4) "NULL"
-- Iteration 9 --
2: settype(): Cannot convert to resource type
bool(false)
bool(false)
string(7) "boolean"
-- Iteration 10 --
2: settype(): Cannot convert to resource type
bool(false)
string(11) "some string"
string(6) "string"
-- Iteration 11 --
2: settype(): Cannot convert to resource type
bool(false)
string(6) "string"
string(6) "string"
-- Iteration 12 --
2: settype(): Cannot convert to resource type
bool(false)
resource(%d) of type (stream)
string(8) "resource"
-- Iteration 13 --
2: settype(): Cannot convert to resource type
bool(false)
resource(%d) of type (stream)
string(8) "resource"
-- Iteration 14 --
2: settype(): Cannot convert to resource type
bool(false)
object(point)#1 (2) {
  ["x"]=>
  int(10)
  ["y"]=>
  int(20)
}
string(6) "object"
2: settype(): Cannot convert to resource type
bool(false)
NULL
string(4) "NULL"
2: settype(): Cannot convert to resource type
bool(false)
NULL
string(4) "NULL"
2: settype(): Cannot convert to resource type
bool(false)
NULL
NULL
string(4) "NULL"
string(4) "NULL"

-- Setting type of data to ?array --
-- Iteration 1 --
bool(true)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
string(5) "array"
-- Iteration 2 --
bool(true)
array(1) {
  [0]=>
  string(14) "another string"
}
string(5) "array"
-- Iteration 3 --
bool(true)
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
}
string(5) "array"
-- Iteration 4 --
bool(true)
array(1) {
  [0]=>
  int(1)
}
string(5) "array"
-- Iteration 5 --
bool(true)
array(1) {
  [0]=>
  int(-20)
}
string(5) "array"
-- Iteration 6 --
bool(true)
array(1) {
  [0]=>
  float(2.54)
}
string(5) "array"
-- Iteration 7 --
bool(true)
array(1) {
  [0]=>
  float(-2.54)
}
string(5) "array"
-- Iteration 8 --
bool(true)
NULL
string(4) "NULL"
-- Iteration 9 --
bool(true)
array(1) {
  [0]=>
  bool(false)
}
string(5) "array"
-- Iteration 10 --
bool(true)
array(1) {
  [0]=>
  string(11) "some string"
}
string(5) "array"
-- Iteration 11 --
bool(true)
array(1) {
  [0]=>
  string(6) "string"
}
string(5) "array"
-- Iteration 12 --
bool(true)
array(1) {
  [0]=>
  resource(%d) of type (stream)
}
string(5) "array"
-- Iteration 13 --
bool(true)
array(1) {
  [0]=>
  resource(%d) of type (stream)
}
string(5) "array"
-- Iteration 14 --
bool(true)
array(2) {
  ["x"]=>
  int(10)
  ["y"]=>
  int(20)
}
string(5) "array"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
NULL
string(4) "NULL"
string(4) "NULL"

-- Setting type of data to ?object --
-- Iteration 1 --
bool(true)
object(stdClass)#2 (3) {
  ["0"]=>
  int(1)
  ["1"]=>
  int(2)
  ["2"]=>
  int(3)
}
string(6) "object"
-- Iteration 2 --
bool(true)
object(stdClass)#2 (1) {
  ["scalar"]=>
  string(14) "another string"
}
string(6) "object"
-- Iteration 3 --
bool(true)
object(stdClass)#2 (3) {
  ["0"]=>
  int(2)
  ["1"]=>
  int(3)
  ["2"]=>
  int(4)
}
string(6) "object"
-- Iteration 4 --
bool(true)
object(stdClass)#2 (1) {
  ["scalar"]=>
  int(1)
}
string(6) "object"
-- Iteration 5 --
bool(true)
object(stdClass)#2 (1) {
  ["scalar"]=>
  int(-20)
}
string(6) "object"
-- Iteration 6 --
bool(true)
object(stdClass)#2 (1) {
  ["scalar"]=>
  float(2.54)
}
string(6) "object"
-- Iteration 7 --
bool(true)
object(stdClass)#2 (1) {
  ["scalar"]=>
  float(-2.54)
}
string(6) "object"
-- Iteration 8 --
bool(true)
NULL
string(4) "NULL"
-- Iteration 9 --
bool(true)
object(stdClass)#2 (1) {
  ["scalar"]=>
  bool(false)
}
string(6) "object"
-- Iteration 10 --
bool(true)
object(stdClass)#2 (1) {
  ["scalar"]=>
  string(11) "some string"
}
string(6) "object"
-- Iteration 11 --
bool(true)
object(stdClass)#2 (1) {
  ["scalar"]=>
  string(6) "string"
}
string(6) "object"
-- Iteration 12 --
bool(true)
object(stdClass)#2 (1) {
  ["scalar"]=>
  resource(%d) of type (stream)
}
string(6) "object"
-- Iteration 13 --
bool(true)
object(stdClass)#2 (1) {
  ["scalar"]=>
  resource(%d) of type (stream)
}
string(6) "object"
-- Iteration 14 --
bool(true)
object(point)#1 (2) {
  ["x"]=>
  int(10)
  ["y"]=>
  int(20)
}
string(6) "object"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
NULL
string(4) "NULL"
string(4) "NULL"

-- Setting type of data to ?string --
-- Iteration 1 --
8: Array to string conversion
bool(true)
string(5) "Array"
string(6) "string"
-- Iteration 2 --
bool(true)
string(14) "another string"
string(6) "string"
-- Iteration 3 --
8: Array to string conversion
bool(true)
string(5) "Array"
string(6) "string"
-- Iteration 4 --
bool(true)
string(1) "1"
string(6) "string"
-- Iteration 5 --
bool(true)
string(3) "-20"
string(6) "string"
-- Iteration 6 --
bool(true)
string(4) "2.54"
string(6) "string"
-- Iteration 7 --
bool(true)
string(5) "-2.54"
string(6) "string"
-- Iteration 8 --
bool(true)
NULL
string(4) "NULL"
-- Iteration 9 --
bool(true)
string(0) ""
string(6) "string"
-- Iteration 10 --
bool(true)
string(11) "some string"
string(6) "string"
-- Iteration 11 --
bool(true)
string(6) "string"
string(6) "string"
-- Iteration 12 --
bool(true)
string(14) "Resource id #%d"
string(6) "string"
-- Iteration 13 --
bool(true)
string(14) "Resource id #%d"
string(6) "string"
-- Iteration 14 --
bool(true)
string(6) "Object"
string(6) "string"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
string(4) "NULL"
bool(true)
NULL
NULL
string(4) "NULL"
string(4) "NULL"
Done
