--TEST--
Test print_r() function
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--INI--
precision=14
--FILE--
<?php

function check_printr( $variables ) {
  $counter = 1;
  foreach( $variables as $variable ) {
    echo "\n-- Iteration $counter --\n";
    //default = false, prints output to screen
    print_r($variable);
    //$return=TRUE, print_r() will return its output, instead of printing it
    $ret_string = print_r($variable, true); //$ret_string captures the output
    echo "\n$ret_string\n";
    //$return=false, print_r() prints the output; default behavior
    print_r($variable, false);
    $counter++;
  }
}

echo "\n*** Testing print_r() on integer variables ***\n";
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
/* calling check_printr() to display contents of integer variables
   using print_r() */
check_printr($integers);

echo "\n*** Testing print_r() on float variables ***\n";
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
/* calling check_printr() to display contents of float variables
   using print_r() */
check_printr($floats);

echo "\n*** Testing print_r() on string variables ***\n";
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
  "1234\t\n5678\n\t9100\"abcda"  // strings with escape characters
);
/* calling check_printr() to display contents of strings using print_r() */
check_printr($strings);

echo "\n*** Testing print_r() on boolean variables ***\n";
$booleans = array (
  TRUE,
  FALSE,
  true,
  false
);
/* calling check_printr() to display boolean variables using print_r() */
check_printr($booleans);
var_dump( reset($booleans) );
echo "\n";
var_dump( current($booleans) );

echo "\n*** Testing print_r() on array variables ***\n";
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
/* calling check_printr() to display contents of $arrays */
check_printr($arrays);

echo "\n*** Testing print_r() on object variables ***\n";
#[AllowDynamicProperties]
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
#[AllowDynamicProperties]
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
/* calling check_printr() to display contents of the objects using print_r() */
check_printr($objects);

echo "\n** Testing print_r() on objects having circular reference **\n";
$recursion_obj1 = new object_class();
$recursion_obj2 = new object_class();
$recursion_obj1->obj = &$recursion_obj2;  //circular reference
$recursion_obj2->obj = &$recursion_obj1;  //circular reference
print_r($recursion_obj2);

echo "\n*** Testing print_r() on resources ***\n";
/* file type resource */
$file_handle = fopen(__FILE__, "r");

/* directory type resource */
$dir_handle = opendir( __DIR__ );

$resources = array (
  $file_handle,
  $dir_handle
);
/* calling check_printr() to display the resource content type
   using print_r() */
check_printr($resources);

echo "\n*** Testing print_r() on different combinations of scalar
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
/* calling check_printr() to display combinations of scalar and
   non-scalar variables using print_r() */
check_printr($variations);

echo "\n*** Testing print_r() on miscellaneous input arguments ***\n";
$misc_values = array (
  @$unset_var,
  NULL,  // NULL argument
  @$undef_variable,  //undefined variable
  null
);
/* calling check_printr() to display miscellaneous data using print_r() */
check_printr($misc_values);

/* closing resource handle used */
closedir($dir_handle);

echo "Done\n";
?>
--EXPECTF--
*** Testing print_r() on integer variables ***

-- Iteration 1 --
0
0
0
-- Iteration 2 --
83
83
83
-- Iteration 3 --
123000000
123000000
123000000
-- Iteration 4 --
-83
-83
-83
-- Iteration 5 --
-12300000
-12300000
-12300000
-- Iteration 6 --
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
    [4] => 5
    [5] => 6
    [6] => 7
    [7] => 8
    [8] => 9
    [9] => 10
)

Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
    [4] => 5
    [5] => 6
    [6] => 7
    [7] => 8
    [8] => 9
    [9] => 10
)

Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
    [4] => 5
    [5] => 6
    [6] => 7
    [7] => 8
    [8] => 9
    [9] => 10
)

-- Iteration 7 --
Array
(
    [0] => -1
    [1] => -2
    [2] => -3
    [3] => -4
    [4] => -5
    [5] => -6
    [6] => -7
    [7] => -8
    [8] => -9
    [9] => -10
)

Array
(
    [0] => -1
    [1] => -2
    [2] => -3
    [3] => -4
    [4] => -5
    [5] => -6
    [6] => -7
    [7] => -8
    [8] => -9
    [9] => -10
)

Array
(
    [0] => -1
    [1] => -2
    [2] => -3
    [3] => -4
    [4] => -5
    [5] => -6
    [6] => -7
    [7] => -8
    [8] => -9
    [9] => -10
)

-- Iteration 8 --
2147483647
2147483647
2147483647
-- Iteration 9 --
2147483648
2147483648
2147483648
-- Iteration 10 --
-2147483648
-2147483648
-2147483648
-- Iteration 11 --
-2147483647
-2147483647
-2147483647
-- Iteration 12 --
2147483647
2147483647
2147483647
-- Iteration 13 --
-2147483648
-2147483648
-2147483648
-- Iteration 14 --
2147483647
2147483647
2147483647
-- Iteration 15 --
-2147483648
-2147483648
-2147483648
*** Testing print_r() on float variables ***

-- Iteration 1 --
-0
-0
-0
-- Iteration 2 --
0
0
0
-- Iteration 3 --
1.234
1.234
1.234
-- Iteration 4 --
-1.234
-1.234
-1.234
-- Iteration 5 --
-2
-2
-2
-- Iteration 6 --
2
2
2
-- Iteration 7 --
-0.5
-0.5
-0.5
-- Iteration 8 --
0.567
0.567
0.567
-- Iteration 9 --
-0.00067
-0.00067
-0.00067
-- Iteration 10 --
-670
-670
-670
-- Iteration 11 --
670
670
670
-- Iteration 12 --
670
670
670
-- Iteration 13 --
-0.00410003
-0.00410003
-0.00410003
-- Iteration 14 --
-4100.03
-4100.03
-4100.03
-- Iteration 15 --
0.004100003
0.004100003
0.004100003
-- Iteration 16 --
4100.003
4100.003
4100.003
-- Iteration 17 --
100000
100000
100000
-- Iteration 18 --
-100000
-100000
-100000
-- Iteration 19 --
1.0E-5
1.0E-5
1.0E-5
-- Iteration 20 --
-1.0E-5
-1.0E-5
-1.0E-5
-- Iteration 21 --
100000
100000
100000
-- Iteration 22 --
-100000
-100000
-100000
-- Iteration 23 --
100000
100000
100000
-- Iteration 24 --
-100000
-100000
-100000
-- Iteration 25 --
100000
100000
100000
-- Iteration 26 --
-100000
-100000
-100000
-- Iteration 27 --
1.0E-5
1.0E-5
1.0E-5
-- Iteration 28 --
-1.0E-5
-1.0E-5
-1.0E-5
-- Iteration 29 --
-2147483649
-2147483649
-2147483649
-- Iteration 30 --
2147483649
2147483649
2147483649
-- Iteration 31 --
2147483649
2147483649
2147483649
-- Iteration 32 --
-2147483649
-2147483649
-2147483649
*** Testing print_r() on string variables ***

-- Iteration 1 --



-- Iteration 2 --



-- Iteration 3 --
 
 
 
-- Iteration 4 --
 
 
 
-- Iteration 5 --
0
0
0
-- Iteration 6 --
%0
%0
%0
-- Iteration 7 --
\0
\0
\0
-- Iteration 8 --
	
	
	
-- Iteration 9 --
\t
\t
\t
-- Iteration 10 --
PHP
PHP
PHP
-- Iteration 11 --
PHP
PHP
PHP
-- Iteration 12 --
abcd%0n1234%005678%000efgh\xijkl
abcd%0n1234%005678%000efgh\xijkl
abcd%0n1234%005678%000efgh\xijkl
-- Iteration 13 --
abcd%0efgh%0ijkl%0mnop%00qrst%0uvwx%00yz
abcd%0efgh%0ijkl%0mnop%00qrst%0uvwx%00yz
abcd%0efgh%0ijkl%0mnop%00qrst%0uvwx%00yz
-- Iteration 14 --
1234	
5678
	9100"abcda
1234	
5678
	9100"abcda
1234	
5678
	9100"abcda
*** Testing print_r() on boolean variables ***

-- Iteration 1 --
1
1
1
-- Iteration 2 --



-- Iteration 3 --
1
1
1
-- Iteration 4 --


bool(true)

bool(true)

*** Testing print_r() on array variables ***

-- Iteration 1 --
Array
(
)

Array
(
)

Array
(
)

-- Iteration 2 --
Array
(
    [0] => 
)

Array
(
    [0] => 
)

Array
(
    [0] => 
)

-- Iteration 3 --
Array
(
    [0] => 
)

Array
(
    [0] => 
)

Array
(
    [0] => 
)

-- Iteration 4 --
Array
(
    [0] => 1
)

Array
(
    [0] => 1
)

Array
(
    [0] => 1
)

-- Iteration 5 --
Array
(
    [0] => 
)

Array
(
    [0] => 
)

Array
(
    [0] => 
)

-- Iteration 6 --
Array
(
    [0] => 
)

Array
(
    [0] => 
)

Array
(
    [0] => 
)

-- Iteration 7 --
Array
(
    [0] => Array
        (
        )

    [1] => Array
        (
        )

)

Array
(
    [0] => Array
        (
        )

    [1] => Array
        (
        )

)

Array
(
    [0] => Array
        (
        )

    [1] => Array
        (
        )

)

-- Iteration 8 --
Array
(
    [0] => Array
        (
            [0] => 1
            [1] => 2
        )

    [1] => Array
        (
            [0] => a
            [1] => b
        )

)

Array
(
    [0] => Array
        (
            [0] => 1
            [1] => 2
        )

    [1] => Array
        (
            [0] => a
            [1] => b
        )

)

Array
(
    [0] => Array
        (
            [0] => 1
            [1] => 2
        )

    [1] => Array
        (
            [0] => a
            [1] => b
        )

)

-- Iteration 9 --
Array
(
    [1] => One
)

Array
(
    [1] => One
)

Array
(
    [1] => One
)

-- Iteration 10 --
Array
(
    [test] => is_array
)

Array
(
    [test] => is_array
)

Array
(
    [test] => is_array
)

-- Iteration 11 --
Array
(
    [0] => 0
)

Array
(
    [0] => 0
)

Array
(
    [0] => 0
)

-- Iteration 12 --
Array
(
    [0] => -1
)

Array
(
    [0] => -1
)

Array
(
    [0] => -1
)

-- Iteration 13 --
Array
(
    [0] => 10.5
    [1] => 5.6
)

Array
(
    [0] => 10.5
    [1] => 5.6
)

Array
(
    [0] => 10.5
    [1] => 5.6
)

-- Iteration 14 --
Array
(
    [0] => string
    [1] => test
)

Array
(
    [0] => string
    [1] => test
)

Array
(
    [0] => string
    [1] => test
)

-- Iteration 15 --
Array
(
    [0] => string
    [1] => test
)

Array
(
    [0] => string
    [1] => test
)

Array
(
    [0] => string
    [1] => test
)

*** Testing print_r() on object variables ***

-- Iteration 1 --
object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)

object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)

object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)

-- Iteration 2 --
no_member_class Object
(
)

no_member_class Object
(
)

no_member_class Object
(
)

-- Iteration 3 --
contains_object_class Object
(
    [p] => 30
    [class_object1] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object2] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object3:contains_object_class:private] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object4:protected] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [no_member_class_object] => no_member_class Object
        (
        )

    [class_object5] => contains_object_class Object
 *RECURSION*
)

contains_object_class Object
(
    [p] => 30
    [class_object1] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object2] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object3:contains_object_class:private] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object4:protected] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [no_member_class_object] => no_member_class Object
        (
        )

    [class_object5] => contains_object_class Object
 *RECURSION*
)

contains_object_class Object
(
    [p] => 30
    [class_object1] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object2] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object3:contains_object_class:private] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object4:protected] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [no_member_class_object] => no_member_class Object
        (
        )

    [class_object5] => contains_object_class Object
 *RECURSION*
)

-- Iteration 4 --
contains_object_class Object
(
    [p] => 30
    [class_object1] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object2] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object3:contains_object_class:private] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object4:protected] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [no_member_class_object] => no_member_class Object
        (
        )

    [class_object5] => contains_object_class Object
 *RECURSION*
)

contains_object_class Object
(
    [p] => 30
    [class_object1] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object2] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object3:contains_object_class:private] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object4:protected] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [no_member_class_object] => no_member_class Object
        (
        )

    [class_object5] => contains_object_class Object
 *RECURSION*
)

contains_object_class Object
(
    [p] => 30
    [class_object1] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object2] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object3:contains_object_class:private] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object4:protected] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [no_member_class_object] => no_member_class Object
        (
        )

    [class_object5] => contains_object_class Object
 *RECURSION*
)

-- Iteration 5 --
object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)

object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)

object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)

-- Iteration 6 --
object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)

object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)

object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)

-- Iteration 7 --
no_member_class Object
(
)

no_member_class Object
(
)

no_member_class Object
(
)

-- Iteration 8 --
object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)

object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)

object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)

-- Iteration 9 --



** Testing print_r() on objects having circular reference **
object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
    [obj] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
            [obj] => object_class Object
 *RECURSION*
        )

)

*** Testing print_r() on resources ***

-- Iteration 1 --
Resource id #5
Resource id #5
Resource id #5
-- Iteration 2 --
Resource id #6
Resource id #6
Resource id #6
*** Testing print_r() on different combinations of scalar
    and non-scalar variables ***

-- Iteration 1 --
Array
(
    [0] => 123
    [1] => -1.2345
    [2] => a
)

Array
(
    [0] => 123
    [1] => -1.2345
    [2] => a
)

Array
(
    [0] => 123
    [1] => -1.2345
    [2] => a
)

-- Iteration 2 --
Array
(
    [0] => d
    [1] => Array
        (
            [0] => 1
            [1] => 3
            [2] => 5
        )

    [2] => 1
    [3] => 
)

Array
(
    [0] => d
    [1] => Array
        (
            [0] => 1
            [1] => 3
            [2] => 5
        )

    [2] => 1
    [3] => 
)

Array
(
    [0] => d
    [1] => Array
        (
            [0] => 1
            [1] => 3
            [2] => 5
        )

    [2] => 1
    [3] => 
)

-- Iteration 3 --
Array
(
    [0] => no_member_class Object
        (
        )

    [1] => Array
        (
        )

    [2] => 
    [3] => 0
)

Array
(
    [0] => no_member_class Object
        (
        )

    [1] => Array
        (
        )

    [2] => 
    [3] => 0
)

Array
(
    [0] => no_member_class Object
        (
        )

    [1] => Array
        (
        )

    [2] => 
    [3] => 0
)

-- Iteration 4 --
Array
(
    [0] => -0
    [1] => Where am I?
    [2] => Array
        (
            [0] => 7
            [1] => 8
            [2] => 9
        )

    [3] => 1
    [4] => A
    [5] => 987654321
)

Array
(
    [0] => -0
    [1] => Where am I?
    [2] => Array
        (
            [0] => 7
            [1] => 8
            [2] => 9
        )

    [3] => 1
    [4] => A
    [5] => 987654321
)

Array
(
    [0] => -0
    [1] => Where am I?
    [2] => Array
        (
            [0] => 7
            [1] => 8
            [2] => 9
        )

    [3] => 1
    [4] => A
    [5] => 987654321
)

-- Iteration 5 --
Array
(
    [0] => 
    [1] => 20000000000
    [2] => 79.1
    [3] => 4.599998
)

Array
(
    [0] => 
    [1] => 20000000000
    [2] => 79.1
    [3] => 4.599998
)

Array
(
    [0] => 
    [1] => 20000000000
    [2] => 79.1
    [3] => 4.599998
)

-- Iteration 6 --
Array
(
    [0] => array(1,2,3,4)1.0000002TRUE
    [1] => 
    [2] => 4611333
    [3] => /00\7
)

Array
(
    [0] => array(1,2,3,4)1.0000002TRUE
    [1] => 
    [2] => 4611333
    [3] => /00\7
)

Array
(
    [0] => array(1,2,3,4)1.0000002TRUE
    [1] => 
    [2] => 4611333
    [3] => /00\7
)

*** Testing print_r() on miscellaneous input arguments ***

-- Iteration 1 --



-- Iteration 2 --



-- Iteration 3 --



-- Iteration 4 --


Done
