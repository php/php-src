--TEST--
Test var_dump() function
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--INI--
precision=14
--FILE--
<?php
/* Prototype: void var_dump ( mixed $expression [, mixed $expression [, $...]] );
   Description: Displays structured information about one or more expressions that includes its type and value.
*/

/* Prototype: void check_vardump( $variables );
   Description: use var_dump() to display the variables */
function check_vardump( $variables ) {
  $counter = 1;
  foreach( $variables as $variable ) {
    echo "-- Iteration $counter --\n";
    var_dump($variable);
    $counter++;
  }
}
  
echo "\n*** Testing var_dump() on integer variables ***\n";
$integers = array ( 
  0,  // zero as argument
  000000123,  //octal value of 83
  123000000,
  -00000123,  //octal value of 83
  -12300000,
  range(1,10),  // positive values
  range(-1,-10),  // negative values
  +2147483647,  // max positive integer
  +2147483648,  // max positive integer + 1
  -2147483648,  // min range of integer
  -2147483647,  // min range of integer + 1
  0x7FFFFFFF,  // max positive hexadecimal integer
  -0x80000000,  // min range of hexadecimal integer
  017777777777,  // max posotive octal integer
  -020000000000  // min range of octal integer
);		    
/* calling check_vardump() to display contents of integer variables 
   using var_dump() */
check_vardump($integers);

echo "\n*** Testing var_dump() on float variables ***\n";
$floats = array (
  -0.0,
  +0.0,
  1.234,
  -1.234,
  -2.000000,
  000002.00,
  -.5,
  .567,
  -.6700000e-3,
  -.6700000E+3,
  .6700000E+3,
  .6700000e+3,
  -4.10003e-3,
  -4.10003E+3,
  4.100003e-3,
  4.100003E+3,
  1e5,
  -1e5,
  1e-5,
  -1e-5,
  1e+5,
  -1e+5,
  1E5,
  -1E5,
  1E+5,
  -1E+5,
  1E-5,
  -1E-5,
  -0x80000001,  // float value, beyond max negative int
  0x80000001,  // float value, beyond max positive int
  020000000001,  // float value, beyond max positive int
  -020000000001  // float value, beyond max negative int 
);
/* calling check_vardump() to display contents of float variables 
   using var_dump() */
check_vardump($floats);

echo "\n*** Testing var_dump() on string variables ***\n";
$strings = array (
  "",
  '',
  " ",
  ' ',
  "0",
  "\0",
  '\0',
  "\t",
  '\t',
  "PHP",
  'PHP',
  "abcd\x0n1234\x0005678\x0000efgh\xijkl",  // strings with hexadecimal NULL
  "abcd\0efgh\0ijkl\x00mnop\x000qrst\00uvwx\0000yz",  // strings with octal NULL
  "1234\t\n5678\n\t9100\rabcda"  // strings with escape characters
);
/* calling check_vardump() to display contents of strings 
   using var_dump() */
check_vardump($strings);

echo "\n*** Testing var_dump() on boolean variables ***\n";
$booleans = array (
  TRUE,
  FALSE,
  true,
  false
);	  
/* calling check_vardump() to display boolean variables
   using var_dump() */
check_vardump($booleans);

echo "\n*** Testing var_dump() on array variables ***\n";
$arrays = array (
  array(),
  array(NULL),
  array(null),
  array(true),
  array(""),
  array(''),
  array(array(), array()),
  array(array(1, 2), array('a', 'b')),
  array(1 => 'One'),
  array("test" => "is_array"),
  array(0),
  array(-1),
  array(10.5, 5.6),
  array("string", "test"),
  array('string', 'test'),
);
/* calling check_vardump() to display contents of an array
   using var_dump() */
check_vardump($arrays);

echo "\n*** Testing var_dump() on object variables ***\n";
class object_class
{
  var       $value;
  public    $public_var1 = 10;
  private   $private_var1 = 20;
  private   $private_var2;
  protected $protected_var1 = "string_1";
  protected $protected_var2;

  function __construct ( ) {
    $this->value = 50;
    $this->public_var2 = 11;
    $this->private_var2 = 21;
    $this->protected_var2 = "string_2";
  }

  public function foo1() {
    echo "foo1() is called\n";
  }
  protected function foo2() {
    echo "foo2() is called\n";
  }
  private function foo3() {
    echo "foo3() is called\n";
  }
}
/* class with no member */
class no_member_class {
 // no members
}

/* class with member as object of other class */
class contains_object_class
{
   var       $p = 30;
   var       $class_object1;
   public    $class_object2;
   private   $class_object3;
   protected $class_object4;
   var       $no_member_class_object;

   public function func() {
     echo "func() is called \n";
   }

   function __construct () {
     $this->class_object1 = new object_class();
     $this->class_object2 = new object_class();
     $this->class_object3 = $this->class_object1;
     $this->class_object4 = $this->class_object2;
     $this->no_member_class_object = new no_member_class();
     $this->class_object5 = $this;   //recursive reference
   }
}

/* objects of different classes */
$obj = new contains_object_class;
$temp_class_obj = new object_class();

/* object which is unset */
$unset_obj = new object_class();
unset($unset_obj);

$objects = array (
  new object_class,
  new no_member_class,
  new contains_object_class,
  $obj,
  $obj->class_object1,
  $obj->class_object2,
  $obj->no_member_class_object,
  $temp_class_obj,
  @$unset_obj
);
/* calling check_vardump() to display contents of the objects
   using var_dump() */
check_vardump($objects);

echo "\n** Testing var_dump() on objects having circular reference **\n";
$recursion_obj1 = new object_class();
$recursion_obj2 = new object_class();
$recursion_obj1->obj = &$recursion_obj2;  //circular reference
$recursion_obj2->obj = &$recursion_obj1;  //circular reference
var_dump($recursion_obj2);

echo "\n*** Testing var_dump() on resources ***\n";
/* file type resource */
$file_handle = fopen(__FILE__, "r"); 

/* directory type resource */
$dir_handle = opendir( dirname(__FILE__) );

$resources = array (
  $file_handle,
  $dir_handle
);
/* calling check_vardump() to display the resource content type
   using var_dump() */
check_vardump($resources);

echo "\n*** Testing var_dump() on different combinations of scalar 
            and non-scalar variables ***\n";
/* a variable which is unset */
$unset_var = 10.5;
unset($unset_var);

/* unset file type resource */
unset($file_handle);

$variations = array (
  array( 123, -1.2345, "a" ),
  array( "d", array(1, 3, 5), true, null),
  array( new no_member_class, array(), false, 0 ),
  array( -0.00, "Where am I?", array(7,8,9), TRUE, 'A', 987654321 ),
  array( @$unset_var, 2.E+10, 100-20.9, 000004.599998 ),  //unusual data
  array( "array(1,2,3,4)1.0000002TRUE", @$file_handle, 111333.00+45e5, '/00\7') 
);
/* calling check_vardump() to display combinations of scalar and
   non-scalar variables using var_dump() */
check_vardump($variations);

echo "\n*** Testing var_dump() on miscelleneous input arguments ***\n";
$misc_values = array (
  @$unset_var,
  NULL,  // NULL argument
  @$undef_variable,  //undefined variable
  null
);
/* calling check_vardump() to display miscelleneous data using var_dump() */
check_vardump($misc_values);

echo "\n*** Testing var_dump() on multiple arguments ***\n";
var_dump( $integers, $floats, $strings, $arrays, $booleans, $resources, 
          $objects, $misc_values, $variations );

/* checking var_dump() on functions */
echo "\n*** Testing var_dump() on anonymous functions ***\n";
$newfunc = create_function('$a,$b', 'return "$a * $b = " . ($a * $b);');
echo "New anonymous function: $newfunc\n";
var_dump( $newfunc(2, 3) );
/* creating anonymous function dynamically */
var_dump( create_function('$a', 'return "$a * $a = " . ($a * $b);') );

echo "\n*** Testing error conditions ***\n";
//passing zero argument
var_dump();

/* closing resource handle used */
closedir($dir_handle); 

echo "Done\n";
?>
--EXPECTF--
*** Testing var_dump() on integer variables ***
-- Iteration 1 --
int(0)
-- Iteration 2 --
int(83)
-- Iteration 3 --
int(123000000)
-- Iteration 4 --
int(-83)
-- Iteration 5 --
int(-12300000)
-- Iteration 6 --
array(10) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
  [9]=>
  int(10)
}
-- Iteration 7 --
array(10) {
  [0]=>
  int(-1)
  [1]=>
  int(-2)
  [2]=>
  int(-3)
  [3]=>
  int(-4)
  [4]=>
  int(-5)
  [5]=>
  int(-6)
  [6]=>
  int(-7)
  [7]=>
  int(-8)
  [8]=>
  int(-9)
  [9]=>
  int(-10)
}
-- Iteration 8 --
int(2147483647)
-- Iteration 9 --
int(2147483648)
-- Iteration 10 --
int(-2147483648)
-- Iteration 11 --
int(-2147483647)
-- Iteration 12 --
int(2147483647)
-- Iteration 13 --
int(-2147483648)
-- Iteration 14 --
int(2147483647)
-- Iteration 15 --
int(-2147483648)

*** Testing var_dump() on float variables ***
-- Iteration 1 --
float(-0)
-- Iteration 2 --
float(0)
-- Iteration 3 --
float(1.234)
-- Iteration 4 --
float(-1.234)
-- Iteration 5 --
float(-2)
-- Iteration 6 --
float(2)
-- Iteration 7 --
float(-0.5)
-- Iteration 8 --
float(0.567)
-- Iteration 9 --
float(-0.00067)
-- Iteration 10 --
float(-670)
-- Iteration 11 --
float(670)
-- Iteration 12 --
float(670)
-- Iteration 13 --
float(-0.00410003)
-- Iteration 14 --
float(-4100.03)
-- Iteration 15 --
float(0.004100003)
-- Iteration 16 --
float(4100.003)
-- Iteration 17 --
float(100000)
-- Iteration 18 --
float(-100000)
-- Iteration 19 --
float(1.0E-5)
-- Iteration 20 --
float(-1.0E-5)
-- Iteration 21 --
float(100000)
-- Iteration 22 --
float(-100000)
-- Iteration 23 --
float(100000)
-- Iteration 24 --
float(-100000)
-- Iteration 25 --
float(100000)
-- Iteration 26 --
float(-100000)
-- Iteration 27 --
float(1.0E-5)
-- Iteration 28 --
float(-1.0E-5)
-- Iteration 29 --
int(-2147483649)
-- Iteration 30 --
int(2147483649)
-- Iteration 31 --
int(2147483649)
-- Iteration 32 --
int(-2147483649)

*** Testing var_dump() on string variables ***
-- Iteration 1 --
string(0) ""
-- Iteration 2 --
string(0) ""
-- Iteration 3 --
string(1) " "
-- Iteration 4 --
string(1) " "
-- Iteration 5 --
string(1) "0"
-- Iteration 6 --
string(1) " "
-- Iteration 7 --
string(2) "\0"
-- Iteration 8 --
string(1) "	"
-- Iteration 9 --
string(2) "\t"
-- Iteration 10 --
string(3) "PHP"
-- Iteration 11 --
string(3) "PHP"
-- Iteration 12 --
string(29) "abcd n1234 05678 00efgh\xijkl"
-- Iteration 13 --
string(34) "abcd efgh ijkl mnop 0qrst uvwx 0yz"
-- Iteration 14 --
string(22) "1234	
5678
	9100abcda"

*** Testing var_dump() on boolean variables ***
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(false)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(false)

*** Testing var_dump() on array variables ***
-- Iteration 1 --
array(0) {
}
-- Iteration 2 --
array(1) {
  [0]=>
  NULL
}
-- Iteration 3 --
array(1) {
  [0]=>
  NULL
}
-- Iteration 4 --
array(1) {
  [0]=>
  bool(true)
}
-- Iteration 5 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 6 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 7 --
array(2) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(0) {
  }
}
-- Iteration 8 --
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
  }
}
-- Iteration 9 --
array(1) {
  [1]=>
  string(3) "One"
}
-- Iteration 10 --
array(1) {
  ["test"]=>
  string(8) "is_array"
}
-- Iteration 11 --
array(1) {
  [0]=>
  int(0)
}
-- Iteration 12 --
array(1) {
  [0]=>
  int(-1)
}
-- Iteration 13 --
array(2) {
  [0]=>
  float(10.5)
  [1]=>
  float(5.6)
}
-- Iteration 14 --
array(2) {
  [0]=>
  string(6) "string"
  [1]=>
  string(4) "test"
}
-- Iteration 15 --
array(2) {
  [0]=>
  string(6) "string"
  [1]=>
  string(4) "test"
}

*** Testing var_dump() on object variables ***
-- Iteration 1 --
object(object_class)#6 (7) {
  ["value"]=>
  int(50)
  ["public_var1"]=>
  int(10)
  ["private_var1":"object_class":private]=>
  int(20)
  ["private_var2":"object_class":private]=>
  int(21)
  ["protected_var1":protected]=>
  string(8) "string_1"
  ["protected_var2":protected]=>
  string(8) "string_2"
  ["public_var2"]=>
  int(11)
}
-- Iteration 2 --
object(no_member_class)#7 (0) {
}
-- Iteration 3 --
object(contains_object_class)#8 (7) {
  ["p"]=>
  int(30)
  ["class_object1"]=>
  object(object_class)#9 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  ["class_object2"]=>
  object(object_class)#10 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  ["class_object3":"contains_object_class":private]=>
  object(object_class)#9 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  ["class_object4":protected]=>
  object(object_class)#10 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  ["no_member_class_object"]=>
  object(no_member_class)#11 (0) {
  }
  ["class_object5"]=>
  *RECURSION*
}
-- Iteration 4 --
object(contains_object_class)#1 (7) {
  ["p"]=>
  int(30)
  ["class_object1"]=>
  object(object_class)#2 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  ["class_object2"]=>
  object(object_class)#3 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  ["class_object3":"contains_object_class":private]=>
  object(object_class)#2 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  ["class_object4":protected]=>
  object(object_class)#3 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  ["no_member_class_object"]=>
  object(no_member_class)#4 (0) {
  }
  ["class_object5"]=>
  *RECURSION*
}
-- Iteration 5 --
object(object_class)#2 (7) {
  ["value"]=>
  int(50)
  ["public_var1"]=>
  int(10)
  ["private_var1":"object_class":private]=>
  int(20)
  ["private_var2":"object_class":private]=>
  int(21)
  ["protected_var1":protected]=>
  string(8) "string_1"
  ["protected_var2":protected]=>
  string(8) "string_2"
  ["public_var2"]=>
  int(11)
}
-- Iteration 6 --
object(object_class)#3 (7) {
  ["value"]=>
  int(50)
  ["public_var1"]=>
  int(10)
  ["private_var1":"object_class":private]=>
  int(20)
  ["private_var2":"object_class":private]=>
  int(21)
  ["protected_var1":protected]=>
  string(8) "string_1"
  ["protected_var2":protected]=>
  string(8) "string_2"
  ["public_var2"]=>
  int(11)
}
-- Iteration 7 --
object(no_member_class)#4 (0) {
}
-- Iteration 8 --
object(object_class)#5 (7) {
  ["value"]=>
  int(50)
  ["public_var1"]=>
  int(10)
  ["private_var1":"object_class":private]=>
  int(20)
  ["private_var2":"object_class":private]=>
  int(21)
  ["protected_var1":protected]=>
  string(8) "string_1"
  ["protected_var2":protected]=>
  string(8) "string_2"
  ["public_var2"]=>
  int(11)
}
-- Iteration 9 --
NULL

** Testing var_dump() on objects having circular reference **
object(object_class)#13 (8) {
  ["value"]=>
  int(50)
  ["public_var1"]=>
  int(10)
  ["private_var1":"object_class":private]=>
  int(20)
  ["private_var2":"object_class":private]=>
  int(21)
  ["protected_var1":protected]=>
  string(8) "string_1"
  ["protected_var2":protected]=>
  string(8) "string_2"
  ["public_var2"]=>
  int(11)
  ["obj"]=>
  &object(object_class)#12 (8) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
    ["obj"]=>
    *RECURSION*
  }
}

*** Testing var_dump() on resources ***
-- Iteration 1 --
resource(5) of type (stream)
-- Iteration 2 --
resource(6) of type (stream)

*** Testing var_dump() on different combinations of scalar 
            and non-scalar variables ***
-- Iteration 1 --
array(3) {
  [0]=>
  int(123)
  [1]=>
  float(-1.2345)
  [2]=>
  string(1) "a"
}
-- Iteration 2 --
array(4) {
  [0]=>
  string(1) "d"
  [1]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(3)
    [2]=>
    int(5)
  }
  [2]=>
  bool(true)
  [3]=>
  NULL
}
-- Iteration 3 --
array(4) {
  [0]=>
  object(no_member_class)#14 (0) {
  }
  [1]=>
  array(0) {
  }
  [2]=>
  bool(false)
  [3]=>
  int(0)
}
-- Iteration 4 --
array(6) {
  [0]=>
  float(-0)
  [1]=>
  string(11) "Where am I?"
  [2]=>
  array(3) {
    [0]=>
    int(7)
    [1]=>
    int(8)
    [2]=>
    int(9)
  }
  [3]=>
  bool(true)
  [4]=>
  string(1) "A"
  [5]=>
  int(987654321)
}
-- Iteration 5 --
array(4) {
  [0]=>
  NULL
  [1]=>
  float(20000000000)
  [2]=>
  float(79.1)
  [3]=>
  float(4.599998)
}
-- Iteration 6 --
array(4) {
  [0]=>
  string(27) "array(1,2,3,4)1.0000002TRUE"
  [1]=>
  NULL
  [2]=>
  float(4611333)
  [3]=>
  string(5) "/00\7"
}

*** Testing var_dump() on miscelleneous input arguments ***
-- Iteration 1 --
NULL
-- Iteration 2 --
NULL
-- Iteration 3 --
NULL
-- Iteration 4 --
NULL

*** Testing var_dump() on multiple arguments ***
array(15) {
  [0]=>
  int(0)
  [1]=>
  int(83)
  [2]=>
  int(123000000)
  [3]=>
  int(-83)
  [4]=>
  int(-12300000)
  [5]=>
  array(10) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
    [4]=>
    int(5)
    [5]=>
    int(6)
    [6]=>
    int(7)
    [7]=>
    int(8)
    [8]=>
    int(9)
    [9]=>
    int(10)
  }
  [6]=>
  array(10) {
    [0]=>
    int(-1)
    [1]=>
    int(-2)
    [2]=>
    int(-3)
    [3]=>
    int(-4)
    [4]=>
    int(-5)
    [5]=>
    int(-6)
    [6]=>
    int(-7)
    [7]=>
    int(-8)
    [8]=>
    int(-9)
    [9]=>
    int(-10)
  }
  [7]=>
  int(2147483647)
  [8]=>
  int(2147483648)
  [9]=>
  int(-2147483648)
  [10]=>
  int(-2147483647)
  [11]=>
  int(2147483647)
  [12]=>
  int(-2147483648)
  [13]=>
  int(2147483647)
  [14]=>
  int(-2147483648)
}
array(32) {
  [0]=>
  float(-0)
  [1]=>
  float(0)
  [2]=>
  float(1.234)
  [3]=>
  float(-1.234)
  [4]=>
  float(-2)
  [5]=>
  float(2)
  [6]=>
  float(-0.5)
  [7]=>
  float(0.567)
  [8]=>
  float(-0.00067)
  [9]=>
  float(-670)
  [10]=>
  float(670)
  [11]=>
  float(670)
  [12]=>
  float(-0.00410003)
  [13]=>
  float(-4100.03)
  [14]=>
  float(0.004100003)
  [15]=>
  float(4100.003)
  [16]=>
  float(100000)
  [17]=>
  float(-100000)
  [18]=>
  float(1.0E-5)
  [19]=>
  float(-1.0E-5)
  [20]=>
  float(100000)
  [21]=>
  float(-100000)
  [22]=>
  float(100000)
  [23]=>
  float(-100000)
  [24]=>
  float(100000)
  [25]=>
  float(-100000)
  [26]=>
  float(1.0E-5)
  [27]=>
  float(-1.0E-5)
  [28]=>
  int(-2147483649)
  [29]=>
  int(2147483649)
  [30]=>
  int(2147483649)
  [31]=>
  int(-2147483649)
}
array(14) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(1) " "
  [3]=>
  string(1) " "
  [4]=>
  string(1) "0"
  [5]=>
  string(1) " "
  [6]=>
  string(2) "\0"
  [7]=>
  string(1) "	"
  [8]=>
  string(2) "\t"
  [9]=>
  string(3) "PHP"
  [10]=>
  string(3) "PHP"
  [11]=>
  string(29) "abcd n1234 05678 00efgh\xijkl"
  [12]=>
  string(34) "abcd efgh ijkl mnop 0qrst uvwx 0yz"
  [13]=>
  string(22) "1234	
5678
	9100abcda"
}
array(15) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(1) {
    [0]=>
    NULL
  }
  [2]=>
  array(1) {
    [0]=>
    NULL
  }
  [3]=>
  array(1) {
    [0]=>
    bool(true)
  }
  [4]=>
  array(1) {
    [0]=>
    string(0) ""
  }
  [5]=>
  array(1) {
    [0]=>
    string(0) ""
  }
  [6]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    array(0) {
    }
  }
  [7]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      int(1)
      [1]=>
      int(2)
    }
    [1]=>
    array(2) {
      [0]=>
      string(1) "a"
      [1]=>
      string(1) "b"
    }
  }
  [8]=>
  array(1) {
    [1]=>
    string(3) "One"
  }
  [9]=>
  array(1) {
    ["test"]=>
    string(8) "is_array"
  }
  [10]=>
  array(1) {
    [0]=>
    int(0)
  }
  [11]=>
  array(1) {
    [0]=>
    int(-1)
  }
  [12]=>
  array(2) {
    [0]=>
    float(10.5)
    [1]=>
    float(5.6)
  }
  [13]=>
  array(2) {
    [0]=>
    string(6) "string"
    [1]=>
    string(4) "test"
  }
  [14]=>
  array(2) {
    [0]=>
    string(6) "string"
    [1]=>
    string(4) "test"
  }
}
array(4) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
  [2]=>
  bool(true)
  [3]=>
  bool(false)
}
array(2) {
  [0]=>
  resource(5) of type (stream)
  [1]=>
  resource(6) of type (stream)
}
array(9) {
  [0]=>
  object(object_class)#6 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  [1]=>
  object(no_member_class)#7 (0) {
  }
  [2]=>
  object(contains_object_class)#8 (7) {
    ["p"]=>
    int(30)
    ["class_object1"]=>
    object(object_class)#9 (7) {
      ["value"]=>
      int(50)
      ["public_var1"]=>
      int(10)
      ["private_var1":"object_class":private]=>
      int(20)
      ["private_var2":"object_class":private]=>
      int(21)
      ["protected_var1":protected]=>
      string(8) "string_1"
      ["protected_var2":protected]=>
      string(8) "string_2"
      ["public_var2"]=>
      int(11)
    }
    ["class_object2"]=>
    object(object_class)#10 (7) {
      ["value"]=>
      int(50)
      ["public_var1"]=>
      int(10)
      ["private_var1":"object_class":private]=>
      int(20)
      ["private_var2":"object_class":private]=>
      int(21)
      ["protected_var1":protected]=>
      string(8) "string_1"
      ["protected_var2":protected]=>
      string(8) "string_2"
      ["public_var2"]=>
      int(11)
    }
    ["class_object3":"contains_object_class":private]=>
    object(object_class)#9 (7) {
      ["value"]=>
      int(50)
      ["public_var1"]=>
      int(10)
      ["private_var1":"object_class":private]=>
      int(20)
      ["private_var2":"object_class":private]=>
      int(21)
      ["protected_var1":protected]=>
      string(8) "string_1"
      ["protected_var2":protected]=>
      string(8) "string_2"
      ["public_var2"]=>
      int(11)
    }
    ["class_object4":protected]=>
    object(object_class)#10 (7) {
      ["value"]=>
      int(50)
      ["public_var1"]=>
      int(10)
      ["private_var1":"object_class":private]=>
      int(20)
      ["private_var2":"object_class":private]=>
      int(21)
      ["protected_var1":protected]=>
      string(8) "string_1"
      ["protected_var2":protected]=>
      string(8) "string_2"
      ["public_var2"]=>
      int(11)
    }
    ["no_member_class_object"]=>
    object(no_member_class)#11 (0) {
    }
    ["class_object5"]=>
    *RECURSION*
  }
  [3]=>
  object(contains_object_class)#1 (7) {
    ["p"]=>
    int(30)
    ["class_object1"]=>
    object(object_class)#2 (7) {
      ["value"]=>
      int(50)
      ["public_var1"]=>
      int(10)
      ["private_var1":"object_class":private]=>
      int(20)
      ["private_var2":"object_class":private]=>
      int(21)
      ["protected_var1":protected]=>
      string(8) "string_1"
      ["protected_var2":protected]=>
      string(8) "string_2"
      ["public_var2"]=>
      int(11)
    }
    ["class_object2"]=>
    object(object_class)#3 (7) {
      ["value"]=>
      int(50)
      ["public_var1"]=>
      int(10)
      ["private_var1":"object_class":private]=>
      int(20)
      ["private_var2":"object_class":private]=>
      int(21)
      ["protected_var1":protected]=>
      string(8) "string_1"
      ["protected_var2":protected]=>
      string(8) "string_2"
      ["public_var2"]=>
      int(11)
    }
    ["class_object3":"contains_object_class":private]=>
    object(object_class)#2 (7) {
      ["value"]=>
      int(50)
      ["public_var1"]=>
      int(10)
      ["private_var1":"object_class":private]=>
      int(20)
      ["private_var2":"object_class":private]=>
      int(21)
      ["protected_var1":protected]=>
      string(8) "string_1"
      ["protected_var2":protected]=>
      string(8) "string_2"
      ["public_var2"]=>
      int(11)
    }
    ["class_object4":protected]=>
    object(object_class)#3 (7) {
      ["value"]=>
      int(50)
      ["public_var1"]=>
      int(10)
      ["private_var1":"object_class":private]=>
      int(20)
      ["private_var2":"object_class":private]=>
      int(21)
      ["protected_var1":protected]=>
      string(8) "string_1"
      ["protected_var2":protected]=>
      string(8) "string_2"
      ["public_var2"]=>
      int(11)
    }
    ["no_member_class_object"]=>
    object(no_member_class)#4 (0) {
    }
    ["class_object5"]=>
    *RECURSION*
  }
  [4]=>
  object(object_class)#2 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  [5]=>
  object(object_class)#3 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  [6]=>
  object(no_member_class)#4 (0) {
  }
  [7]=>
  object(object_class)#5 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  [8]=>
  NULL
}
array(4) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  NULL
  [3]=>
  NULL
}
array(6) {
  [0]=>
  array(3) {
    [0]=>
    int(123)
    [1]=>
    float(-1.2345)
    [2]=>
    string(1) "a"
  }
  [1]=>
  array(4) {
    [0]=>
    string(1) "d"
    [1]=>
    array(3) {
      [0]=>
      int(1)
      [1]=>
      int(3)
      [2]=>
      int(5)
    }
    [2]=>
    bool(true)
    [3]=>
    NULL
  }
  [2]=>
  array(4) {
    [0]=>
    object(no_member_class)#14 (0) {
    }
    [1]=>
    array(0) {
    }
    [2]=>
    bool(false)
    [3]=>
    int(0)
  }
  [3]=>
  array(6) {
    [0]=>
    float(-0)
    [1]=>
    string(11) "Where am I?"
    [2]=>
    array(3) {
      [0]=>
      int(7)
      [1]=>
      int(8)
      [2]=>
      int(9)
    }
    [3]=>
    bool(true)
    [4]=>
    string(1) "A"
    [5]=>
    int(987654321)
  }
  [4]=>
  array(4) {
    [0]=>
    NULL
    [1]=>
    float(20000000000)
    [2]=>
    float(79.1)
    [3]=>
    float(4.599998)
  }
  [5]=>
  array(4) {
    [0]=>
    string(27) "array(1,2,3,4)1.0000002TRUE"
    [1]=>
    NULL
    [2]=>
    float(4611333)
    [3]=>
    string(5) "/00\7"
  }
}

*** Testing var_dump() on anonymous functions ***
New anonymous function:  lambda_1
string(9) "2 * 3 = 6"
string(9) " lambda_2"

*** Testing error conditions ***

Warning: var_dump() expects at least 1 parameter, 0 given in %s on line %d
Done

