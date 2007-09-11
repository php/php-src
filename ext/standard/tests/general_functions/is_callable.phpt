--TEST--
Test is_callable() function
--INI--
precision=14
error_reporting = E_ALL & ~E_NOTICE | E_STRICT
--FILE--
<?php
/* Prototype: bool is_callable ( mixed $var [, bool $syntax_only [, string &$callable_name]] );
   Description: Verify that the contents of a variable can be called as a function
                In case of objects, $var = array($SomeObject, 'MethodName')
*/

/* Prototype: void check_iscallable( $functions );
   Description: use iscallable() on given string to check for valid function name
                returns true if valid function name, false otherwise
*/
function check_iscallable( $functions ) {
  $counter = 1;
  foreach($functions as $func) {
    echo "-- Iteration  $counter --\n";
    var_dump( is_callable($func) );  //given only $var argument
    var_dump( is_callable($func, TRUE) );  //given $var and $syntax argument
    var_dump( is_callable($func, TRUE, $callable_name) );
    var_dump( is_callable($func, FALSE) );  //given $var and $syntax argument
    var_dump( is_callable($func, FALSE, $callable_name) );
    echo $callable_name, "\n";
    $counter++;
  }
}

/* Prototype: void check_iscallable_objects( $methods );
   Description: use is_callable() on given $method to check if the array 
                contains a valid method name;
                returns true if valid function name, false otherwise
*/
function check_iscallable_objects( $methods ) {
  global $loop_counter;
  $counter = 1;
  foreach($methods as $method) {
    echo "-- Innerloop iteration $counter of Outerloop iteration $loop_counter --\n";
    var_dump( is_callable($method) );
    var_dump( is_callable($method, true) );
    var_dump( is_callable($method, false) );
    var_dump( is_callable($method, true, $callable_name) );
    var_dump( is_callable($method, false, $callable_name) );
    echo $callable_name, "\n";
    $counter++;
  }
}

echo "\n*** Testing is_callable() on defined functions ***\n";
/* function name with simple string */
function someFunction() {
}

/* function name with mixed string and integer */
function x123() {
}

/* function name as NULL */
function NULL() {
}

/* function name with boolean name */
function false() {
}

/* function name with string and special character */
function Hello_World() {
}

$defined_functions = array (
  $functionVar1 = 'someFunction',
  $functionVar2 = 'x123',
  $functionVar3 = 'NULL',
  $functionVar4 = 'false',
  $functionVar5 = "Hello_World"
);
/* use check_iscallable() to check whether given string is valid function name
 *  expected: true as it is valid callback
 */
check_iscallable($defined_functions);

echo "\n*** Testing is_callable() on undefined functions ***\n";
$undef_functions = array (
  "",  //empty string
  '',
  " ",  //string with a space
  ' ',
  "12356",
  "\0",
  '\0',
  "hello world",
  'hello world',
  "welcome\0",
  'welcome\0',
  "==%%%***$$$@@@!!",
  "false",
  "\070",
  '\t',  //escape character
  '\007',
  '123',
  'echo()'
);
/* use check_iscallable() to check whether given string is valid function name
 * expected: true with $syntax = TRUE
 *           false with $syntax = FALSE
 */
check_iscallable($undef_functions);

echo "\n*** Testing is_callable() on objects ***\n";
class object_class
{
  public $value = 100;
  
  /* static method */
  static public function foo() {
  }
  
  public function foo1() {
  }
  /* function name with mixed string and integer */
  public function x123() {
  }
  /* function name as NULL */
  public function null() {
  }
  /* function name having boolean value */
  public function TRUE() {
  }  

  protected function foo2() {
  }
  private function foo3() {
  }
}
/* class with no member */
class no_member_class {
 // no members
}
/* class with member as object of other class */
class contains_object_class
{
   public    $class_object1;
   var       $no_member_class_object;

   public function func() {
     echo "func() is called \n";
   }

   function contains_object_class () {
     $this->class_object1 = new object_class();
     $this->no_member_class_object = new no_member_class();
   }
}
/* objects of different classes */
$obj = new contains_object_class;
$temp_class_obj = new object_class();

/* object which is unset */
$unset_obj = new object_class();
unset($unset_obj);

/* check is_callable() on static method */
echo "\n** Testing behavior of is_callable() on static methods **\n";
var_dump( is_callable('object_class::foo()', true) );   //expected: true
var_dump( is_callable('object_class::foo()') );    //expected: false

echo "\n** Testing normal operations of is_callable() on objects **\n";
$objects = array (
  new object_class,
  new no_member_class,
  new contains_object_class,
  $obj,
  $obj->class_object1,
  $obj->no_member_class_object,
  $temp_class_obj,
  @$unset_obj
);

/* loop to check whether given object/string has valid given method name
 *  expected: true if valid callback
 *            false otherwise
 */
$loop_counter = 1;
foreach($objects as $object) {
  echo "--- Outerloop iteration $loop_counter ---\n";
  $methods = array (
    array( $object, 'foo1' ),
    array( $object, 'foo2' ),
    array( $object, 'foo3' ),
    array( $object, 'x123' ),
    array( $object, 'null' ),
    array( $object, 'TRUE' ),
    array( $object, '123' ),
    array( @$temp_class_obj->value, 100 ),
    array( $object, 'func' ),
    array( 'object_class', 'foo1' ),
  );
  /* use check_iscallable_objects() to check whether given object/string
     has valid method name */
  check_iscallable_objects($methods);
  $loop_counter++;
}

echo "\n*** Testing is_callable() on invalid function names ***\n";
/* check on unset variables */
$unset_var = 10;
unset ($unset_var);

/* opening file resource type */
$file_handle = fopen (__FILE__, "r");

$variants = array (
  NULL,  // NULL as argument
  0,  // zero as argument
  1234567890,  // positive value
  -100123456782,  // negative value
  -2.000000,  // negative float value
  .567,  // positive float value
  FALSE,  // boolean value
  array(1, 2, 3),  // array
  @$unset_var,
  @$undef_var,  //undefined variable
  $file_handle
);
/* use check_iscallable() to check whether given variable is valid function name
 *  expected: false
 */
check_iscallable($variants);

echo "\n*** Testing error conditions ***\n";
/* passing zero argument */
var_dump( is_callable() );
/* passing more than required arguments */
var_dump( is_callable("string", TRUE, $callable_name, "EXTRA") );

/* closing resources used */
fclose($file_handle);

echo "Done\n";
?>
--EXPECTF--
*** Testing is_callable() on defined functions ***
-- Iteration  1 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
someFunction
-- Iteration  2 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
x123
-- Iteration  3 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
NULL
-- Iteration  4 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
false
-- Iteration  5 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Hello_World

*** Testing is_callable() on undefined functions ***
-- Iteration  1 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)

-- Iteration  2 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)

-- Iteration  3 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
 
-- Iteration  4 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
 
-- Iteration  5 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
12356
-- Iteration  6 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)

-- Iteration  7 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
\0
-- Iteration  8 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
hello world
-- Iteration  9 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
hello world
-- Iteration  10 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
welcome
-- Iteration  11 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
welcome\0
-- Iteration  12 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
==%%%***$$$@@@!!
-- Iteration  13 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
false
-- Iteration  14 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
8
-- Iteration  15 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
\t
-- Iteration  16 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
\007
-- Iteration  17 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
123
-- Iteration  18 --
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
echo()

*** Testing is_callable() on objects ***

** Testing behavior of is_callable() on static methods **
bool(true)
bool(false)

** Testing normal operations of is_callable() on objects **
--- Outerloop iteration 1 ---
-- Innerloop iteration 1 of Outerloop iteration 1 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
object_class::foo1
-- Innerloop iteration 2 of Outerloop iteration 1 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
object_class::foo2
-- Innerloop iteration 3 of Outerloop iteration 1 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
object_class::foo3
-- Innerloop iteration 4 of Outerloop iteration 1 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
object_class::x123
-- Innerloop iteration 5 of Outerloop iteration 1 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
object_class::null
-- Innerloop iteration 6 of Outerloop iteration 1 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
object_class::TRUE
-- Innerloop iteration 7 of Outerloop iteration 1 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
object_class::123
-- Innerloop iteration 8 of Outerloop iteration 1 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 9 of Outerloop iteration 1 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
object_class::func
-- Innerloop iteration 10 of Outerloop iteration 1 --

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
object_class::foo1
--- Outerloop iteration 2 ---
-- Innerloop iteration 1 of Outerloop iteration 2 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::foo1
-- Innerloop iteration 2 of Outerloop iteration 2 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::foo2
-- Innerloop iteration 3 of Outerloop iteration 2 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::foo3
-- Innerloop iteration 4 of Outerloop iteration 2 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::x123
-- Innerloop iteration 5 of Outerloop iteration 2 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::null
-- Innerloop iteration 6 of Outerloop iteration 2 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::TRUE
-- Innerloop iteration 7 of Outerloop iteration 2 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::123
-- Innerloop iteration 8 of Outerloop iteration 2 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 9 of Outerloop iteration 2 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::func
-- Innerloop iteration 10 of Outerloop iteration 2 --

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
object_class::foo1
--- Outerloop iteration 3 ---
-- Innerloop iteration 1 of Outerloop iteration 3 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::foo1
-- Innerloop iteration 2 of Outerloop iteration 3 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::foo2
-- Innerloop iteration 3 of Outerloop iteration 3 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::foo3
-- Innerloop iteration 4 of Outerloop iteration 3 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::x123
-- Innerloop iteration 5 of Outerloop iteration 3 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::null
-- Innerloop iteration 6 of Outerloop iteration 3 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::TRUE
-- Innerloop iteration 7 of Outerloop iteration 3 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::123
-- Innerloop iteration 8 of Outerloop iteration 3 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 9 of Outerloop iteration 3 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
contains_object_class::func
-- Innerloop iteration 10 of Outerloop iteration 3 --

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
object_class::foo1
--- Outerloop iteration 4 ---
-- Innerloop iteration 1 of Outerloop iteration 4 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::foo1
-- Innerloop iteration 2 of Outerloop iteration 4 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::foo2
-- Innerloop iteration 3 of Outerloop iteration 4 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::foo3
-- Innerloop iteration 4 of Outerloop iteration 4 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::x123
-- Innerloop iteration 5 of Outerloop iteration 4 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::null
-- Innerloop iteration 6 of Outerloop iteration 4 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::TRUE
-- Innerloop iteration 7 of Outerloop iteration 4 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
contains_object_class::123
-- Innerloop iteration 8 of Outerloop iteration 4 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 9 of Outerloop iteration 4 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
contains_object_class::func
-- Innerloop iteration 10 of Outerloop iteration 4 --

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
object_class::foo1
--- Outerloop iteration 5 ---
-- Innerloop iteration 1 of Outerloop iteration 5 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
object_class::foo1
-- Innerloop iteration 2 of Outerloop iteration 5 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
object_class::foo2
-- Innerloop iteration 3 of Outerloop iteration 5 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
object_class::foo3
-- Innerloop iteration 4 of Outerloop iteration 5 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
object_class::x123
-- Innerloop iteration 5 of Outerloop iteration 5 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
object_class::null
-- Innerloop iteration 6 of Outerloop iteration 5 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
object_class::TRUE
-- Innerloop iteration 7 of Outerloop iteration 5 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
object_class::123
-- Innerloop iteration 8 of Outerloop iteration 5 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 9 of Outerloop iteration 5 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
object_class::func
-- Innerloop iteration 10 of Outerloop iteration 5 --

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
object_class::foo1
--- Outerloop iteration 6 ---
-- Innerloop iteration 1 of Outerloop iteration 6 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::foo1
-- Innerloop iteration 2 of Outerloop iteration 6 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::foo2
-- Innerloop iteration 3 of Outerloop iteration 6 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::foo3
-- Innerloop iteration 4 of Outerloop iteration 6 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::x123
-- Innerloop iteration 5 of Outerloop iteration 6 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::null
-- Innerloop iteration 6 of Outerloop iteration 6 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::TRUE
-- Innerloop iteration 7 of Outerloop iteration 6 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::123
-- Innerloop iteration 8 of Outerloop iteration 6 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 9 of Outerloop iteration 6 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
no_member_class::func
-- Innerloop iteration 10 of Outerloop iteration 6 --

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
object_class::foo1
--- Outerloop iteration 7 ---
-- Innerloop iteration 1 of Outerloop iteration 7 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
object_class::foo1
-- Innerloop iteration 2 of Outerloop iteration 7 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
object_class::foo2
-- Innerloop iteration 3 of Outerloop iteration 7 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
object_class::foo3
-- Innerloop iteration 4 of Outerloop iteration 7 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
object_class::x123
-- Innerloop iteration 5 of Outerloop iteration 7 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
object_class::null
-- Innerloop iteration 6 of Outerloop iteration 7 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
object_class::TRUE
-- Innerloop iteration 7 of Outerloop iteration 7 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
object_class::123
-- Innerloop iteration 8 of Outerloop iteration 7 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 9 of Outerloop iteration 7 --
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
object_class::func
-- Innerloop iteration 10 of Outerloop iteration 7 --

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
object_class::foo1
--- Outerloop iteration 8 ---
-- Innerloop iteration 1 of Outerloop iteration 8 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 2 of Outerloop iteration 8 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 3 of Outerloop iteration 8 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 4 of Outerloop iteration 8 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 5 of Outerloop iteration 8 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 6 of Outerloop iteration 8 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 7 of Outerloop iteration 8 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 8 of Outerloop iteration 8 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 9 of Outerloop iteration 8 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Innerloop iteration 10 of Outerloop iteration 8 --

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
bool(true)

Strict Standards: Non-static method object_class::foo1() cannot be called statically in %s on line %d
bool(true)
object_class::foo1

*** Testing is_callable() on invalid function names ***
-- Iteration  1 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- Iteration  2 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
0
-- Iteration  3 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
1234567890
-- Iteration  4 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
-100123456782
-- Iteration  5 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
-2
-- Iteration  6 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
0.567
-- Iteration  7 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- Iteration  8 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Array
-- Iteration  9 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- Iteration  10 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- Iteration  11 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Resource id #5

*** Testing error conditions ***

Warning: Wrong parameter count for is_callable() in %s on line %d
NULL

Warning: Wrong parameter count for is_callable() in %s on line %d
NULL
Done
