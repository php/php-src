--TEST--
Test extract() function 
--FILE--
<?php
/* Prototype: int extract( array var_array[, int extract_type[, string prefix]] )
 * Description: Import variables into the current symbol table from an array
 */

/* various combinations of arrays to be used for the test */
$mixed_array = array(
  array(),
  array( 1,2,3,4,5,6,7,8,9 ),
  array( "One", "Two", "Three", "Four", "Five" ),
  array( 6, "six", 7, "seven", 8, "eight", 9, "nine" ),
  array( "a" => "aaa", "A" => "AAA", "c" => "ccc", "d" => "ddd", "e" => "eee" ),
  array( "1" => "one", "2" => "two", "3" => "three", "4" => "four", "5" => "five" ),
  array( 1 => "one", 2 => "two", 3 => 7, 4 => "four", 5 => "five" ),
  array( "f" => "fff", "1" => "one", 4 => 6, "" => "blank", 2.4 => "float", "F" => "FFF",
         "blank" => "", 3.7 => 3.7, 5.4 => 7, 6 => 8.6, '5' => "Five", "4name" => "jonny", "a" => NULL, NULL => 3 ),
  array( 12, "name", 'age', '45' ),
  array( array("oNe", "tWo", 4), array(10, 20, 30, 40, 50), array() ),
  array( "one" => 1, "one" => 2, "three" => 3, 3, 4, 3 => 33, 4 => 44, 5, 6,
          5.4 => 54, 5.7 => 57, "5.4" => 554, "5.7" => 557 )
);

$val = 4;
$str = "John";

/* Extracting Global Variables */
extract($GLOBALS, EXTR_REFS);

/* Testing Error Conditions */
echo "*** Testing Error Conditions ***\n";

/* Zero Arguments */
var_dump( extract() );

/* Invalid second argument ( only 0-6 is valid) */
var_dump( extract($mixed_array[7], -1 . "wddr") );
var_dump( extract($mixed_array[7], 7 , "wddr") );

/* scalar argument */
var_dump( extract($val) );

/* string argument */
var_dump( extract($str) );

/* More than valid number of arguments i.e. 3 args */
var_dump( extract($mixed_array[7], EXTR_SKIP, "aa", "ee") );

/* Two Arguments, second as prefix but without prefix string as third argument */
var_dump( extract($mixed_array[7],EXTR_PREFIX_IF_EXISTS) );

$counter = 0;

foreach ( $mixed_array as $sub_array ) {
  echo "\n-- Iteration $counter --\n";
  $counter++;

  var_dump ( extract($sub_array)); /* Single Argument */

  /* variations of two arguments */
  var_dump ( extract($sub_array, EXTR_OVERWRITE));
  var_dump ( extract($sub_array, EXTR_SKIP));
  var_dump ( extract($sub_array, EXTR_IF_EXISTS));

  /* variations of three arguments with use of various extract types*/
  var_dump ( extract($sub_array, EXTR_PREFIX_INVALID, "ssd"));
  var_dump ( extract($sub_array, EXTR_PREFIX_SAME, "sss"));
  var_dump ( extract($sub_array, EXTR_PREFIX_ALL, "bb"));
  var_dump ( extract($sub_array, EXTR_PREFIX_ALL, ""));  // "_" taken as default prefix 
  var_dump ( extract($sub_array, EXTR_PREFIX_IF_EXISTS, "bb"));
}


/* EXTR_REFS as second Argument */
$a = array ('foo' => 'aaa');
var_dump ( extract($a, EXTR_REFS));
$b = $a;
$b['foo'] = 'bbb';
var_dump ( extract($a, EXTR_REFS));

/* EXTR_PREFIX_ALL called twice with same prefix string */
echo "\n*** Testing for EXTR_PREFIX_ALL called twice with same prefix string ***\n";
var_dump ( extract($mixed_array[5], EXTR_PREFIX_ALL, "same"));
var_dump ( extract($mixed_array[7], EXTR_PREFIX_ALL, "same"));
var_dump ( extract($mixed_array[7], EXTR_PREFIX_ALL, "diff"));

/* To show variables with numerical prefixes cannot be extracted */
$var["i"] = 1;
$var["j"] = 2;
$var["k"] = 3;
echo "\n*** Testing for Numerical prefixes ***\n";
var_dump(extract($var));


$var1["m"] = 1;
$var1[2] = 2;
$var1[] = 3;
var_dump ( extract($var1));


/* Using Class and objects */

echo "\n*** Testing for object ***\n";
class classA
{
  public  $v;
}

$A = new classA();
var_dump ( extract(get_object_vars($A),EXTR_REFS));

echo "\nDone";
?>

--EXPECTF--
*** Testing Error Conditions ***

Warning: Wrong parameter count for extract() in %s on line %d
NULL

Warning: extract(): Unknown extract type in %s on line %d
NULL

Warning: extract(): Unknown extract type in %s on line %d
NULL

Warning: extract(): First argument should be an array in %s on line %d
NULL

Warning: extract(): First argument should be an array in %s on line %d
NULL

Warning: Wrong parameter count for extract() in %s on line %d
NULL

Warning: extract(): Prefix expected to be specified in %s on line %d
NULL

-- Iteration 0 --
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)

-- Iteration 1 --
int(0)
int(0)
int(0)
int(0)
int(9)
int(0)
int(9)
int(9)
int(0)

-- Iteration 2 --
int(0)
int(0)
int(0)
int(0)
int(5)
int(0)
int(5)
int(5)
int(0)

-- Iteration 3 --
int(0)
int(0)
int(0)
int(0)
int(8)
int(0)
int(8)
int(8)
int(0)

-- Iteration 4 --
int(5)
int(5)
int(0)
int(5)
int(5)
int(5)
int(5)
int(5)
int(5)

-- Iteration 5 --
int(0)
int(0)
int(0)
int(0)
int(5)
int(0)
int(5)
int(5)
int(0)

-- Iteration 6 --
int(0)
int(0)
int(0)
int(0)
int(5)
int(0)
int(5)
int(5)
int(0)

-- Iteration 7 --
int(4)
int(4)
int(0)
int(4)
int(12)
int(4)
int(11)
int(11)
int(4)

-- Iteration 8 --
int(0)
int(0)
int(0)
int(0)
int(4)
int(0)
int(4)
int(4)
int(0)

-- Iteration 9 --
int(0)
int(0)
int(0)
int(0)
int(3)
int(0)
int(3)
int(3)
int(0)

-- Iteration 10 --
int(2)
int(2)
int(0)
int(2)
int(8)
int(2)
int(8)
int(8)
int(2)
int(1)
int(1)

*** Testing for EXTR_PREFIX_ALL called twice with same prefix string ***
int(5)
int(11)
int(11)

*** Testing for Numerical prefixes ***
int(3)
int(1)

*** Testing for object ***
int(1)

Done
