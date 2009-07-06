--TEST--
Test unset(), empty() and isset() functions
--FILE--
<?php
/* Prototype: void unset ( mixed $var [, mixed $var [, mixed $...]] );
   Description: unset() destroys the specified variables

   Prototype: bool empty( mixed $var );
   Description: Determine whether a variable is considered to be empty

   Prototype: bool isset ( mixed $var [, mixed $var [, $...]] );
   Description: Returns TRUE if var exists; FALSE otherwise
*/

echo "*** Testing unset(), empty() & isset() with scalar variables ***\n";

// testing scalar variables
$scalar_variables = array(
  0,    
  1,
  +1
  -1,
  0x55,
  -0xFA,
  0123,
  -0563,
  0.0,
  1e5,
  1E-5,
  -1.5e5,
  +5.6,
  "",
  '',
  " ",
  ' ',
  "string",
  "123",
  "0",
  "ture",
  "FALSE",
  "NULL",
  "null",
  true,
  false,
  TRUE,
  FALSE
);

$loop_counter = 1;
foreach ($scalar_variables as $scalar_var) {
  $set_var = 10; // this variable to use with isset
  echo "-- Iteration $loop_counter --\n"; $loop_counter++;

  // checking with isset before unsetting, expected: bool(true)
  var_dump( isset($scalar_var) ); 
  var_dump( isset($scalar_var, $set_var) );  
  // checking if the var is empty, expected: bool(false) on most
  // except "", 0, "0", NULL, FALSE
  var_dump( empty($scalar_var) );  
  
  // destroy the variable using unset
  unset( $scalar_var );   
  // dump and see if its destroyed, expcted: NULL 
  var_dump( $scalar_var ); 

  // check using isset to see if unset, expected: bool(false)
  var_dump( isset($scalar_var) ); 
  var_dump( isset($scalar_var, $set_var) ); 

  // empty to check if empty, expecting bool(true)
  var_dump( empty($scalar_var) ); 

  // isset() with two args, one arg only unset, expected: bool(false)
  var_dump( isset($scalar_var, $set_var) );

  // isset() with two args, both args already unset, expected: bool(false);
  unset($set_var);
  var_dump( isset($scalar_var, $set_var) );
}

echo "\n*** Testing unset(), empty() & isset() with arrays ***\n";
$array_variables = array(
  array(),
  array(NULL),
  array(0),
  array("0"),
  array(""),
  array(1,2,3,4),
  array(1.4,2.5,5.6),
  array(1 => "One", 2 => "two"),
  array("Name" => "Jack", "Age" => "30"),
  array(1,2, "One" => "1", 2 => "two", ""=>"empty", "" => '')
);  

$outer_loop_counter = 1;
foreach ($array_variables as $array_var) {
  echo "--- Outerloop Iteration $outer_loop_counter ---\n";
  
  // check the isset and unset on non existing key
  $var = 1;  // a var which is defined
  // try to unset the element which is non-existent
  unset($array_var['non_existent']); 
  // check using isset() & empty() on a non_existent element in the array
  var_dump( isset($array_var['non_existent']) );
  var_dump( isset($array_var['non_existent'], $var) );
  var_dump( isset($array_var['non_existent'], $array_var['none']) );
  var_dump( empty($array_var['non_existent']) );

  // testing empty and isset on arrays 
  var_dump( empty($array_var) ); // expecting bool(false), except: array(), which is considered empty
  var_dump( isset($array_var) ); // expecting bool(true), except: array(), which is not set

  // get the keys of the $array_var 
  $keys = array_keys($array_var);
  // unset each element in the array and see the working of unset, isset & empty
  $inner_loop_counter = 1;
  foreach ($keys as $key_value) {
    echo "-- Innerloop Iteration $inner_loop_counter of Outerloop Iteration $outer_loop_counter --\n"; 
    $inner_loop_counter++;

    // unset the element
    unset($array_var[$key_value]); 
    // dump the array after element was unset
    var_dump($array_var);
    // check using isset for the element that was unset
    var_dump( isset($array_var[$key_val]) ); // expected: bool(false)
    // calling isset with more args
    var_dump( isset($array_var[$key_val], $array_var) ); //expected: bool(false)

    // calling empty, expected bool(true) 
    var_dump( empty($array_var[$key_val]) );

    // dump the array to see that that array did not get modified 
    // because of using isset, empty and unset on its element
    var_dump($array_var);
  }

  $outer_loop_counter++;

  // unset the whole array
  unset($array_var);
  // dump the array to see its unset
  var_dump($array_var);
  // use isset to see that array is not set
  var_dump( isset($array_var) ); //expected: bool(false)
  var_dump( isset($array_var, $array_var[$key_val]) ); // expected: bool(false)
  
  // empty() to see if the array is empty
  var_dump( empty($array_var) ); // expected: bool(true)
}

echo "\n*** Testing unset(), emtpy() & isset() with resource variables ***\n";
$fp = fopen(__FILE__, "r");
$dfp = opendir( dirname(__FILE__) );
$resources = array (
  $fp,
  $dfp
);
$loop_counter = 1;
foreach ($resources as $resource) {
  $temp_var = 10;
  echo "-- Iteration $loop_counter --\n"; $loop_counter++;
  //dump the resource first
  var_dump($resource);

  // check using isset() and empty()
  var_dump( isset($resource) );  // expected: bool(true)
  var_dump( empty($resource) );  // expected: bool(false)
  // call isset() with two args, both set
  var_dump( isset($resource, $temp_var) ); // expected: bool(true)

  // dump the resource to see using isset() and empty () had no effect on it
  var_dump($resource);

  // unset the resource
  unset($resource);
  // check using isset() and empty()
  var_dump( isset($resource) );  // expected: bool(flase)
  var_dump( empty($resource) );  // expected: bool(true)
  // call isset() with two args, but one set
  var_dump( isset($resource, $temp_var) ); // expected: bool(false)
  // uset the temp_var
  unset($temp_var);
  // now the isset() with both the args as unset
  var_dump( isset($resource, $temp_var) ); // expected: bool(false);
  
  // dump the resource to see if there any effect on it 
  var_dump($resource);
}
// unset and dump the array containing all the resources to see that
// unset works correctly 
unset($resources);
var_dump($resources);
var_dump( isset($resources) );  //expected: bool(false)
var_dump( empty($resources) );  // expected: bool(true)

echo "\n*** Testing unset(), empty() & isset() with objects ***\n";
class Point
{
  var $x;
  var $y;
  var $lable;
  
  function Point($x, $y) {
    $this->x = $x;
    $this->y = $y;
  }
  
  function setLable($lable) {
    $this->lable = $lable;
  }
  function testPoint() {
    echo "\nPoint::testPoint() called\n";
  }
}
$point1 = new Point(30,40);

// use unset/empty/isset to check the object 
var_dump($point1); // dump the object 

// check the object and member that is not set
var_dump( isset($point1) );  // expected: bool(true)
var_dump( empty($point1) );  // expected: bool(false)
var_dump( isset($point1->$lable) );  //expected: bool(flase)
var_dump( empty($point1->$lable) );  //expected: bool(true)

//set the member variable lable and check
$point1->setLable("Point1");
var_dump( isset($point1->$lable) );  //expected: bool(true)
var_dump( empty($point1->$lable) );  //expected: bool(false)

// dump the object to see that obj was not harmed 
// because of the usage of the isset & empty
var_dump($point1);

//unset a member and check
unset($point1->x);
// dump the point to see that variable was unset
var_dump($point1);
var_dump( isset($point1->x) );  // expected: bool(false)
var_dump( empty($point1->x) );  // expected: bool(true)

// unset all members and check
unset($point1->y);
unset($point1->lable);
// dump the objec to check that all variables are unset
var_dump($point1);
var_dump( isset($point1) );  // expected: bool(ture)
var_dump( empty($point1) );  // expected: bool(false)

//unset the object and check
unset($point1);
var_dump( isset($point1) );  // expected: bool(false)
var_dump( empty($point1) );  // expected: bool(true)
// dump to see that object is unset
var_dump($point1);

// try isset/unset/empty on a member function
$point2 = new Point(5,6);
var_dump( isset($point2->testPoint) );
var_dump( empty($point2->testPoint) );
unset($point2->testPoint);
var_dump( isset($point2->testPoint) );
var_dump( empty($point2->testPoint) );

// use get_class_methods to see effect if any
var_dump( get_class_methods($point2) );
// dump the object to see the effect, none expected
var_dump($point2);

/* testing variation in operation for isset(), empty() & unset().
Note: Most of the variation for function unset() is testing by a
      set of testcases named "Zend/tests/unset_cv??.phpt", only 
      variation not tested are attempted here */

echo "\n*** Testing possible variation in operation for isset(), empty() & unset() ***\n";
/* unset() variation1: checking unset on static variable inside a function. 
 * unset() destroys the variable only in the context of the rest of a function
 * Following calls will restore the previous value of a variable.
 */
echo "\n** Testing unset() variation 1: unset on static variable inside a function **\n";
function test_unset1() {
  static $static_var;
  
  // increment the value of the static. this change is in function context
  $static_var ++;
 
  echo "value of static_var before unset: $static_var\n"; 
  // check using isset and empty 
  var_dump( isset($static_var) );
  var_dump( empty($static_var) );
  
  // unset the static var
  unset($static_var);
  echo "value of static_var after unset: $static_var\n"; 
  // check using isset and empty 
  var_dump( isset($static_var) );
  var_dump( empty($static_var) );

  // assign a value to static var
  $static_var = 20;
  echo "value of static_var after new assignment: $static_var\n"; 
}
// call the functiont
test_unset1();
test_unset1();
test_unset1();


echo "\n** Testing unset() variation 2: unset on a variable passed by ref. inside of a function **\n";
/* unset() variation2: Pass by reference  
 * If a variable that is PASSED BY REFERENCE is unset() inside of a function, 
 * only the local variable is destroyed. The variable in the calling environment
 * will retain the same value as before unset()  was called. 
 */
function test_unset2( &$ref_val ) {
  // unset the variable passed
  unset($ref_val);
  // check using isset and empty to confirm
  var_dump( isset($ref_val) );
  var_dump( empty($ref_val) );

  // set the value ot a new one
  $ref_val = "new value by ref";
}

$value = "value";
var_dump($value);
test_unset2($value);
var_dump($value);

 
echo "\n** Testing unset() variation 3: unset on a global variable inside of a function **\n";
/* unset() variation2: unset on a global variable inside a function
 * If a globalized variable is unset() inside of a function, only the
 * local variable is destroyed. The variable in the calling environment
 * will retain the same value as before unset() was called.
 */
$global_var = 10;

function test_unset3() {
  global $global_var;
   
  // check the $global_var using isset and empty 
  var_dump( isset($global_var) ); 
  var_dump( empty($global_var) ); 
 
  // unset the global var
  unset($global_var);
 
  // check the $global_var using isset and empty 
  var_dump( isset($global_var) ); 
  var_dump( empty($global_var) ); 
}

var_dump($global_var);
test_unset3();
var_dump($global_var);

//Note: No error conditions relating to passing arugments can be tested
// because these are not functions but statements, it will result in syntax error.
?>
===DONE===
--EXPECTF--
*** Testing unset(), empty() & isset() with scalar variables ***
-- Iteration 1 --
bool(true)
bool(true)
bool(true)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 2 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 3 --
bool(true)
bool(true)
bool(true)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 4 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 5 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 6 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 7 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 8 --
bool(true)
bool(true)
bool(true)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 9 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 10 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 11 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 12 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 13 --
bool(true)
bool(true)
bool(true)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 14 --
bool(true)
bool(true)
bool(true)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 15 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 16 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 17 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 18 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 19 --
bool(true)
bool(true)
bool(true)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 20 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 21 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 22 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 23 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 24 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 25 --
bool(true)
bool(true)
bool(true)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 26 --
bool(true)
bool(true)
bool(false)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
-- Iteration 27 --
bool(true)
bool(true)
bool(true)

Notice: Undefined variable: scalar_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)

*** Testing unset(), empty() & isset() with arrays ***
--- Outerloop Iteration 1 ---
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)

Notice: Undefined variable: array_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
--- Outerloop Iteration 2 ---
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
-- Innerloop Iteration 1 of Outerloop Iteration 2 --
array(0) {
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(0) {
}

Notice: Undefined variable: array_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
--- Outerloop Iteration 3 ---
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
-- Innerloop Iteration 1 of Outerloop Iteration 3 --
array(0) {
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(0) {
}

Notice: Undefined variable: array_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
--- Outerloop Iteration 4 ---
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
-- Innerloop Iteration 1 of Outerloop Iteration 4 --
array(0) {
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(0) {
}

Notice: Undefined variable: array_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
--- Outerloop Iteration 5 ---
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
-- Innerloop Iteration 1 of Outerloop Iteration 5 --
array(0) {
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(0) {
}

Notice: Undefined variable: array_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
--- Outerloop Iteration 6 ---
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
-- Innerloop Iteration 1 of Outerloop Iteration 6 --
array(3) {
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(3) {
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
-- Innerloop Iteration 2 of Outerloop Iteration 6 --
array(2) {
  [2]=>
  int(3)
  [3]=>
  int(4)
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(2) {
  [2]=>
  int(3)
  [3]=>
  int(4)
}
-- Innerloop Iteration 3 of Outerloop Iteration 6 --
array(1) {
  [3]=>
  int(4)
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(1) {
  [3]=>
  int(4)
}
-- Innerloop Iteration 4 of Outerloop Iteration 6 --
array(0) {
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(0) {
}

Notice: Undefined variable: array_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
--- Outerloop Iteration 7 ---
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
-- Innerloop Iteration 1 of Outerloop Iteration 7 --
array(2) {
  [1]=>
  float(2.5)
  [2]=>
  float(5.6)
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(2) {
  [1]=>
  float(2.5)
  [2]=>
  float(5.6)
}
-- Innerloop Iteration 2 of Outerloop Iteration 7 --
array(1) {
  [2]=>
  float(5.6)
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(1) {
  [2]=>
  float(5.6)
}
-- Innerloop Iteration 3 of Outerloop Iteration 7 --
array(0) {
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(0) {
}

Notice: Undefined variable: array_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
--- Outerloop Iteration 8 ---
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
-- Innerloop Iteration 1 of Outerloop Iteration 8 --
array(1) {
  [2]=>
  string(3) "two"
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(1) {
  [2]=>
  string(3) "two"
}
-- Innerloop Iteration 2 of Outerloop Iteration 8 --
array(0) {
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(0) {
}

Notice: Undefined variable: array_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
--- Outerloop Iteration 9 ---
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
-- Innerloop Iteration 1 of Outerloop Iteration 9 --
array(1) {
  ["Age"]=>
  string(2) "30"
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(1) {
  ["Age"]=>
  string(2) "30"
}
-- Innerloop Iteration 2 of Outerloop Iteration 9 --
array(0) {
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(0) {
}

Notice: Undefined variable: array_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
--- Outerloop Iteration 10 ---
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
-- Innerloop Iteration 1 of Outerloop Iteration 10 --
array(4) {
  [1]=>
  int(2)
  ["One"]=>
  string(1) "1"
  [2]=>
  string(3) "two"
  [""]=>
  string(0) ""
}

Notice: Undefined variable: key_val in %s on line %d
bool(true)

Notice: Undefined variable: key_val in %s on line %d
bool(true)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(4) {
  [1]=>
  int(2)
  ["One"]=>
  string(1) "1"
  [2]=>
  string(3) "two"
  [""]=>
  string(0) ""
}
-- Innerloop Iteration 2 of Outerloop Iteration 10 --
array(3) {
  ["One"]=>
  string(1) "1"
  [2]=>
  string(3) "two"
  [""]=>
  string(0) ""
}

Notice: Undefined variable: key_val in %s on line %d
bool(true)

Notice: Undefined variable: key_val in %s on line %d
bool(true)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(3) {
  ["One"]=>
  string(1) "1"
  [2]=>
  string(3) "two"
  [""]=>
  string(0) ""
}
-- Innerloop Iteration 3 of Outerloop Iteration 10 --
array(2) {
  [2]=>
  string(3) "two"
  [""]=>
  string(0) ""
}

Notice: Undefined variable: key_val in %s on line %d
bool(true)

Notice: Undefined variable: key_val in %s on line %d
bool(true)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(2) {
  [2]=>
  string(3) "two"
  [""]=>
  string(0) ""
}
-- Innerloop Iteration 4 of Outerloop Iteration 10 --
array(1) {
  [""]=>
  string(0) ""
}

Notice: Undefined variable: key_val in %s on line %d
bool(true)

Notice: Undefined variable: key_val in %s on line %d
bool(true)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(1) {
  [""]=>
  string(0) ""
}
-- Innerloop Iteration 5 of Outerloop Iteration 10 --
array(0) {
}

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(false)

Notice: Undefined variable: key_val in %s on line %d
bool(true)
array(0) {
}

Notice: Undefined variable: array_var in %s on line %d
NULL
bool(false)
bool(false)
bool(true)

*** Testing unset(), emtpy() & isset() with resource variables ***
-- Iteration 1 --
resource(%d) of type (stream)
bool(true)
bool(false)
bool(true)
resource(%d) of type (stream)
bool(false)
bool(true)
bool(false)
bool(false)

Notice: Undefined variable: resource in %s on line %d
NULL
-- Iteration 2 --
resource(%d) of type (stream)
bool(true)
bool(false)
bool(true)
resource(%d) of type (stream)
bool(false)
bool(true)
bool(false)
bool(false)

Notice: Undefined variable: resource in %s on line %d
NULL

Notice: Undefined variable: resources in %s on line %d
NULL
bool(false)
bool(true)

*** Testing unset(), empty() & isset() with objects ***
object(Point)#%d (3) {
  ["x"]=>
  int(30)
  ["y"]=>
  int(40)
  ["lable"]=>
  NULL
}
bool(true)
bool(false)

Notice: Undefined variable: lable in %s on line %d
bool(false)

Notice: Undefined variable: lable in %s on line %d
bool(true)

Notice: Undefined variable: lable in %s on line %d
bool(false)

Notice: Undefined variable: lable in %s on line %d
bool(true)
object(Point)#%d (3) {
  ["x"]=>
  int(30)
  ["y"]=>
  int(40)
  ["lable"]=>
  string(6) "Point1"
}
object(Point)#%d (2) {
  ["y"]=>
  int(40)
  ["lable"]=>
  string(6) "Point1"
}
bool(false)
bool(true)
object(Point)#%d (0) {
}
bool(true)
bool(false)
bool(false)
bool(true)

Notice: Undefined variable: point1 in %s on line %d
NULL
bool(false)
bool(true)
bool(false)
bool(true)
array(3) {
  [0]=>
  string(5) "Point"
  [1]=>
  string(8) "setLable"
  [2]=>
  string(9) "testPoint"
}
object(Point)#%d (3) {
  ["x"]=>
  int(5)
  ["y"]=>
  int(6)
  ["lable"]=>
  NULL
}

*** Testing possible variation in operation for isset(), empty() & unset() ***

** Testing unset() variation 1: unset on static variable inside a function **
value of static_var before unset: 1
bool(true)
bool(false)

Notice: Undefined variable: static_var in %s on line %d
value of static_var after unset: 
bool(false)
bool(true)
value of static_var after new assignment: 20
value of static_var before unset: 2
bool(true)
bool(false)

Notice: Undefined variable: static_var in %s on line %d
value of static_var after unset: 
bool(false)
bool(true)
value of static_var after new assignment: 20
value of static_var before unset: 3
bool(true)
bool(false)

Notice: Undefined variable: static_var in %s on line %d
value of static_var after unset: 
bool(false)
bool(true)
value of static_var after new assignment: 20

** Testing unset() variation 2: unset on a variable passed by ref. inside of a function **
string(5) "value"
bool(false)
bool(true)
string(5) "value"

** Testing unset() variation 3: unset on a global variable inside of a function **
int(10)
bool(true)
bool(false)
bool(false)
bool(true)
int(10)
===DONE===
