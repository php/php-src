--TEST--
Test var_export() function with locale
--INI--
serialize_precision=17
--SKIPIF--
<?php
if (!setlocale(LC_ALL, "german", "de","de_DE","de_DE.ISO8859-1","de_DE.ISO_8859-1","de_DE.UTF-8")) {
        die("skip locale needed for this test is not supported on this platform");
}
if (PHP_INT_SIZE < 8) {
        die("skip 64-bit only");
}
?>
--FILE--
<?php
setlocale(LC_ALL, "german", "de","de_DE","de_DE.ISO8859-1","de_DE.ISO_8859-1","de_DE.UTF-8");
echo "*** Testing var_export() with integer values ***\n";
// different integer values
$valid_ints = array(
                '0',
                '1',
                '-1',
                '-2147483648', // max negative integer value
                '-2147483647',
                2147483647,  // max positive integer value
                2147483640,
                0x123B,      // integer as hexadecimal
                '0x12ab',
                '0Xfff',
                '0XFA',
                -0x7fffffff - 1, // max negative integer as hexadecimal
                '0x7fffffff',  // max positive integer as hexadecimal
                0x7FFFFFFF,  // max positive integer as hexadecimal
                '0123',        // integer as octal
                01,       // should be quivalent to octal 1
                -017777777777 - 1, // max negative integer as octal
                017777777777,  // max positive integer as octal
               );
$counter = 1;
/* Loop to check for above integer values with var_export() */
echo "\n*** Output for integer values ***\n";
foreach($valid_ints as $int_value) {
echo "\nIteration ".$counter."\n";
var_export( $int_value );
echo "\n";
var_export( $int_value, FALSE);
echo "\n";
var_dump( var_export( $int_value, TRUE) );
echo "\n";
$counter++;
}

echo "*** Testing var_export() with valid boolean values ***\n";
// different valid  boolean values
$valid_bool = array(
            1,
            TRUE,
                true,
                0,
            FALSE,
            false
               );
$counter = 1;
/* Loop to check for above boolean values with var_export() */
echo "\n*** Output for boolean values ***\n";
foreach($valid_bool as $bool_value) {
echo "\nIteration ".$counter."\n";
var_export( $bool_value );
echo "\n";
var_export( $bool_value, FALSE);
echo "\n";
var_dump( var_export( $bool_value, TRUE) );
echo "\n";
$counter++;
}

echo "*** Testing var_export() with valid float values ***\n";
// different valid  float values
$valid_floats = array(
  (float)-2147483649, // float value
  (float)2147483648,  // float value
  (float)-0x80000001, // float value, beyond max negative int
  (float)0x800000001, // float value, beyond max positive int
  (float)020000000001, // float value, beyond max positive int
  (float)-020000000001, // float value, beyond max negative int
  0.0,
  -0.1,
  10.0000000000000000005,
  10.5e+5,
  1e5,
  1e-5,
  1e+5,
  1E5,
  1E+5,
  1E-5,
  .5e+7,
  .6e-19,
  .05E+44,
  .0034E-30
);
$counter = 1;
/* Loop to check for above float values with var_export() */
echo "\n*** Output for float values ***\n";
foreach($valid_floats as $float_value) {
echo "\nIteration ".$counter."\n";
var_export( $float_value );
echo "\n";
var_export( $float_value, FALSE);
echo "\n";
var_dump( var_export( $float_value, TRUE) );
echo "\n";
$counter++;
}

echo "*** Testing var_export() with valid strings ***\n";
// different valid  string
$valid_strings = array(
            "",
            " ",
            '',
            ' ',
            "string",
            'string',
            "NULL",
            'null',
            "FALSE",
            'false',
            "\x0b",
            "\0",
            '\0',
            '\060',
            "\070"
          );
$counter = 1;
/* Loop to check for above strings with var_export() */
echo "\n*** Output for strings ***\n";
foreach($valid_strings as $str) {
echo "\nIteration ".$counter."\n";
var_export( $str );
echo "\n";
var_export( $str, FALSE);
echo "\n";
var_dump( var_export( $str, TRUE) );
echo "\n";
$counter++;
}

echo "*** Testing var_export() with valid arrays ***\n";
// different valid  arrays
$valid_arrays = array(
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
           array('string', 'test')
          );
$counter = 1;
/* Loop to check for above arrays with var_export() */
echo "\n*** Output for arrays ***\n";
foreach($valid_arrays as $arr) {
echo "\nIteration ".$counter."\n";
var_export( $arr );
echo "\n";
var_export( $arr, FALSE);
echo "\n";
var_dump( var_export( $arr, TRUE) );
echo "\n";
$counter++;
}

echo "*** Testing var_export() with valid objects ***\n";

// class with no members
class foo
{
// no members
}

// abstract class
abstract class abstractClass
{
  abstract protected function getClassName();
  public function printClassName () {
    echo $this->getClassName() . "\n";
  }
}
// implement abstract class
class concreteClass extends abstractClass
{
  protected function getClassName() {
    return "concreteClass";
  }
}

// interface class
interface iValue
{
   public function setVal ($name, $val);
   public function dumpVal ();
}
// implement the interface
class Value implements iValue
{
  private $vars = array ();

  public function setVal ( $name, $val ) {
    $this->vars[$name] = $val;
  }

  public function dumpVal () {
    var_export ( $vars );
  }
}

// a gereral class
class myClass
{
  var $foo_object;
  public $public_var;
  public $public_var1;
  private $private_var;
  protected $protected_var;

  function __construct ( ) {
    $this->foo_object = new foo();
    $this->public_var = 10;
    $this->public_var1 = new foo();
    $this->private_var = new foo();
    $this->proected_var = new foo();
  }
}

// create a object of each class defined above
$myClass_object = new myClass();
$foo_object = new foo();
$Value_object = new Value();
$concreteClass_object = new concreteClass();

$valid_objects = array(
                  new stdclass,
                  new foo,
                  new concreteClass,
                  new Value,
                  new myClass,
                  $myClass_object,
                  $myClass_object->foo_object,
                  $myClass_object->public_var1,
                  $foo_object,
                  $Value_object,
                  $concreteClass_object
                 );
 $counter = 1;
/* Loop to check for above objects with var_export() */
echo "\n*** Output for objects ***\n";
foreach($valid_objects as $obj) {
echo "\nIteration ".$counter."\n";
var_export( $obj );
echo "\n";
var_export( $obj, FALSE);
echo "\n";
var_dump( var_export( $obj, TRUE) );
echo "\n";
$counter++;
}

echo "*** Testing var_export() with valid null values ***\n";
// different valid  null values
$unset_var = array();
unset ($unset_var); // now a null
$null_var = NULL;

$valid_nulls = array(
                NULL,
                null,
                $null_var,
               );
 $counter = 1;
/* Loop to check for above null values with var_export() */
echo "\n*** Output for null values ***\n";
foreach($valid_nulls as $null_value) {
echo "\nIteration ".$counter."\n";
var_export( $null_value );
echo "\n";
var_export( $null_value, FALSE);
echo "\n";
var_dump( var_export( $null_value, true) );
echo "\n";
$counter++;
}

echo "\nDone";


?>
--EXPECT--
*** Testing var_export() with integer values ***

*** Output for integer values ***

Iteration 1
'0'
'0'
string(3) "'0'"


Iteration 2
'1'
'1'
string(3) "'1'"


Iteration 3
'-1'
'-1'
string(4) "'-1'"


Iteration 4
'-2147483648'
'-2147483648'
string(13) "'-2147483648'"


Iteration 5
'-2147483647'
'-2147483647'
string(13) "'-2147483647'"


Iteration 6
2147483647
2147483647
string(10) "2147483647"


Iteration 7
2147483640
2147483640
string(10) "2147483640"


Iteration 8
4667
4667
string(4) "4667"


Iteration 9
'0x12ab'
'0x12ab'
string(8) "'0x12ab'"


Iteration 10
'0Xfff'
'0Xfff'
string(7) "'0Xfff'"


Iteration 11
'0XFA'
'0XFA'
string(6) "'0XFA'"


Iteration 12
-2147483648
-2147483648
string(11) "-2147483648"


Iteration 13
'0x7fffffff'
'0x7fffffff'
string(12) "'0x7fffffff'"


Iteration 14
2147483647
2147483647
string(10) "2147483647"


Iteration 15
'0123'
'0123'
string(6) "'0123'"


Iteration 16
1
1
string(1) "1"


Iteration 17
-2147483648
-2147483648
string(11) "-2147483648"


Iteration 18
2147483647
2147483647
string(10) "2147483647"

*** Testing var_export() with valid boolean values ***

*** Output for boolean values ***

Iteration 1
1
1
string(1) "1"


Iteration 2
true
true
string(4) "true"


Iteration 3
true
true
string(4) "true"


Iteration 4
0
0
string(1) "0"


Iteration 5
false
false
string(5) "false"


Iteration 6
false
false
string(5) "false"

*** Testing var_export() with valid float values ***

*** Output for float values ***

Iteration 1
-2147483649.0
-2147483649.0
string(13) "-2147483649.0"


Iteration 2
2147483648.0
2147483648.0
string(12) "2147483648.0"


Iteration 3
-2147483649.0
-2147483649.0
string(13) "-2147483649.0"


Iteration 4
34359738369.0
34359738369.0
string(13) "34359738369.0"


Iteration 5
2147483649.0
2147483649.0
string(12) "2147483649.0"


Iteration 6
-2147483649.0
-2147483649.0
string(13) "-2147483649.0"


Iteration 7
0.0
0.0
string(3) "0.0"


Iteration 8
-0.10000000000000001
-0.10000000000000001
string(20) "-0.10000000000000001"


Iteration 9
10.0
10.0
string(4) "10.0"


Iteration 10
1050000.0
1050000.0
string(9) "1050000.0"


Iteration 11
100000.0
100000.0
string(8) "100000.0"


Iteration 12
1.0000000000000001E-5
1.0000000000000001E-5
string(21) "1.0000000000000001E-5"


Iteration 13
100000.0
100000.0
string(8) "100000.0"


Iteration 14
100000.0
100000.0
string(8) "100000.0"


Iteration 15
100000.0
100000.0
string(8) "100000.0"


Iteration 16
1.0000000000000001E-5
1.0000000000000001E-5
string(21) "1.0000000000000001E-5"


Iteration 17
5000000.0
5000000.0
string(9) "5000000.0"


Iteration 18
6.0000000000000006E-20
6.0000000000000006E-20
string(22) "6.0000000000000006E-20"


Iteration 19
5.0000000000000001E+42
5.0000000000000001E+42
string(22) "5.0000000000000001E+42"


Iteration 20
3.4000000000000001E-33
3.4000000000000001E-33
string(22) "3.4000000000000001E-33"

*** Testing var_export() with valid strings ***

*** Output for strings ***

Iteration 1
''
''
string(2) "''"


Iteration 2
' '
' '
string(3) "' '"


Iteration 3
''
''
string(2) "''"


Iteration 4
' '
' '
string(3) "' '"


Iteration 5
'string'
'string'
string(8) "'string'"


Iteration 6
'string'
'string'
string(8) "'string'"


Iteration 7
'NULL'
'NULL'
string(6) "'NULL'"


Iteration 8
'null'
'null'
string(6) "'null'"


Iteration 9
'FALSE'
'FALSE'
string(7) "'FALSE'"


Iteration 10
'false'
'false'
string(7) "'false'"


Iteration 11
''
''
string(3) "''"


Iteration 12
'' . "\0" . ''
'' . "\0" . ''
string(14) "'' . "\0" . ''"


Iteration 13
'\\0'
'\\0'
string(5) "'\\0'"


Iteration 14
'\\060'
'\\060'
string(7) "'\\060'"


Iteration 15
'8'
'8'
string(3) "'8'"

*** Testing var_export() with valid arrays ***

*** Output for arrays ***

Iteration 1
array (
)
array (
)
string(9) "array (
)"


Iteration 2
array (
  0 => NULL,
)
array (
  0 => NULL,
)
string(22) "array (
  0 => NULL,
)"


Iteration 3
array (
  0 => NULL,
)
array (
  0 => NULL,
)
string(22) "array (
  0 => NULL,
)"


Iteration 4
array (
  0 => true,
)
array (
  0 => true,
)
string(22) "array (
  0 => true,
)"


Iteration 5
array (
  0 => '',
)
array (
  0 => '',
)
string(20) "array (
  0 => '',
)"


Iteration 6
array (
  0 => '',
)
array (
  0 => '',
)
string(20) "array (
  0 => '',
)"


Iteration 7
array (
  0 => 
  array (
  ),
  1 => 
  array (
  ),
)
array (
  0 => 
  array (
  ),
  1 => 
  array (
  ),
)
string(55) "array (
  0 => 
  array (
  ),
  1 => 
  array (
  ),
)"


Iteration 8
array (
  0 => 
  array (
    0 => 1,
    1 => 2,
  ),
  1 => 
  array (
    0 => 'a',
    1 => 'b',
  ),
)
array (
  0 => 
  array (
    0 => 1,
    1 => 2,
  ),
  1 => 
  array (
    0 => 'a',
    1 => 'b',
  ),
)
string(107) "array (
  0 => 
  array (
    0 => 1,
    1 => 2,
  ),
  1 => 
  array (
    0 => 'a',
    1 => 'b',
  ),
)"


Iteration 9
array (
  1 => 'One',
)
array (
  1 => 'One',
)
string(23) "array (
  1 => 'One',
)"


Iteration 10
array (
  'test' => 'is_array',
)
array (
  'test' => 'is_array',
)
string(33) "array (
  'test' => 'is_array',
)"


Iteration 11
array (
  0 => 0,
)
array (
  0 => 0,
)
string(19) "array (
  0 => 0,
)"


Iteration 12
array (
  0 => -1,
)
array (
  0 => -1,
)
string(20) "array (
  0 => -1,
)"


Iteration 13
array (
  0 => 10.5,
  1 => 5.5999999999999996,
)
array (
  0 => 10.5,
  1 => 5.5999999999999996,
)
string(49) "array (
  0 => 10.5,
  1 => 5.5999999999999996,
)"


Iteration 14
array (
  0 => 'string',
  1 => 'test',
)
array (
  0 => 'string',
  1 => 'test',
)
string(41) "array (
  0 => 'string',
  1 => 'test',
)"


Iteration 15
array (
  0 => 'string',
  1 => 'test',
)
array (
  0 => 'string',
  1 => 'test',
)
string(41) "array (
  0 => 'string',
  1 => 'test',
)"

*** Testing var_export() with valid objects ***

*** Output for objects ***

Iteration 1
(object) array(
)
(object) array(
)
string(17) "(object) array(
)"


Iteration 2
foo::__set_state(array(
))
foo::__set_state(array(
))
string(26) "foo::__set_state(array(
))"


Iteration 3
concreteClass::__set_state(array(
))
concreteClass::__set_state(array(
))
string(36) "concreteClass::__set_state(array(
))"


Iteration 4
Value::__set_state(array(
   'vars' => 
  array (
  ),
))
Value::__set_state(array(
   'vars' => 
  array (
  ),
))
string(57) "Value::__set_state(array(
   'vars' => 
  array (
  ),
))"


Iteration 5
myClass::__set_state(array(
   'foo_object' => 
  foo::__set_state(array(
  )),
   'public_var' => 10,
   'public_var1' => 
  foo::__set_state(array(
  )),
   'private_var' => 
  foo::__set_state(array(
  )),
   'protected_var' => NULL,
   'proected_var' => 
  foo::__set_state(array(
  )),
))
myClass::__set_state(array(
   'foo_object' => 
  foo::__set_state(array(
  )),
   'public_var' => 10,
   'public_var1' => 
  foo::__set_state(array(
  )),
   'private_var' => 
  foo::__set_state(array(
  )),
   'protected_var' => NULL,
   'proected_var' => 
  foo::__set_state(array(
  )),
))
string(293) "myClass::__set_state(array(
   'foo_object' => 
  foo::__set_state(array(
  )),
   'public_var' => 10,
   'public_var1' => 
  foo::__set_state(array(
  )),
   'private_var' => 
  foo::__set_state(array(
  )),
   'protected_var' => NULL,
   'proected_var' => 
  foo::__set_state(array(
  )),
))"


Iteration 6
myClass::__set_state(array(
   'foo_object' => 
  foo::__set_state(array(
  )),
   'public_var' => 10,
   'public_var1' => 
  foo::__set_state(array(
  )),
   'private_var' => 
  foo::__set_state(array(
  )),
   'protected_var' => NULL,
   'proected_var' => 
  foo::__set_state(array(
  )),
))
myClass::__set_state(array(
   'foo_object' => 
  foo::__set_state(array(
  )),
   'public_var' => 10,
   'public_var1' => 
  foo::__set_state(array(
  )),
   'private_var' => 
  foo::__set_state(array(
  )),
   'protected_var' => NULL,
   'proected_var' => 
  foo::__set_state(array(
  )),
))
string(293) "myClass::__set_state(array(
   'foo_object' => 
  foo::__set_state(array(
  )),
   'public_var' => 10,
   'public_var1' => 
  foo::__set_state(array(
  )),
   'private_var' => 
  foo::__set_state(array(
  )),
   'protected_var' => NULL,
   'proected_var' => 
  foo::__set_state(array(
  )),
))"


Iteration 7
foo::__set_state(array(
))
foo::__set_state(array(
))
string(26) "foo::__set_state(array(
))"


Iteration 8
foo::__set_state(array(
))
foo::__set_state(array(
))
string(26) "foo::__set_state(array(
))"


Iteration 9
foo::__set_state(array(
))
foo::__set_state(array(
))
string(26) "foo::__set_state(array(
))"


Iteration 10
Value::__set_state(array(
   'vars' => 
  array (
  ),
))
Value::__set_state(array(
   'vars' => 
  array (
  ),
))
string(57) "Value::__set_state(array(
   'vars' => 
  array (
  ),
))"


Iteration 11
concreteClass::__set_state(array(
))
concreteClass::__set_state(array(
))
string(36) "concreteClass::__set_state(array(
))"

*** Testing var_export() with valid null values ***

*** Output for null values ***

Iteration 1
NULL
NULL
string(4) "NULL"


Iteration 2
NULL
NULL
string(4) "NULL"


Iteration 3
NULL
NULL
string(4) "NULL"


Done
