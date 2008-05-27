--TEST--
Test strrev() function : usage variations - unexpected inputs 
--FILE--
<?php
/* Prototype  : string strrev(string $str);
 * Description: Reverse a string 
 * Source code: ext/standard/string.c
*/

/* Testing strrev() function with unexpected inputs for 'str' */

echo "*** Testing strrev() : unexpected inputs for 'str' ***\n";
//class declaration
class sample {
  public function __toString(){
    return "object";
  }
}

//get the resource 
$resource = fopen(__FILE__, "r");

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array(

  // int data
  0,
  1,
  12345,
  -2345,

  // float data
  10.5,
   -10.5,
  10.5e10,
  10.6E-10,
  .5,

  // array data
  array(),
  array(0),
  array(1),
  array(1, 2),
  array('color' => 'red', 'item' => 'pen'),

  // null data
  NULL,
  null,

  // boolean data
  true,
  false,
  TRUE,
  FALSE,

  // empty data
  "",
  '',

  // object data
  new sample(),

  // resource
  $resource,

  // undefined data
  @$undefined_var,

  // unset data
  @$unset_var
);

// loop through each element of the array for str

$count = 1;
foreach($values as $value) {
  echo "\n-- Iterator $count --\n";
  var_dump( strrev($value) );
  $count++;
};

fclose($resource);  //closing the file handle

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrev() : unexpected inputs for 'str' ***

-- Iterator 1 --
unicode(1) "0"

-- Iterator 2 --
unicode(1) "1"

-- Iterator 3 --
unicode(5) "54321"

-- Iterator 4 --
unicode(5) "5432-"

-- Iterator 5 --
unicode(4) "5.01"

-- Iterator 6 --
unicode(5) "5.01-"

-- Iterator 7 --
unicode(12) "000000000501"

-- Iterator 8 --
unicode(7) "9-E60.1"

-- Iterator 9 --
unicode(3) "5.0"

-- Iterator 10 --

Warning: strrev() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

-- Iterator 11 --

Warning: strrev() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

-- Iterator 12 --

Warning: strrev() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

-- Iterator 13 --

Warning: strrev() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

-- Iterator 14 --

Warning: strrev() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

-- Iterator 15 --
unicode(0) ""

-- Iterator 16 --
unicode(0) ""

-- Iterator 17 --
unicode(1) "1"

-- Iterator 18 --
unicode(0) ""

-- Iterator 19 --
unicode(1) "1"

-- Iterator 20 --
unicode(0) ""

-- Iterator 21 --
unicode(0) ""

-- Iterator 22 --
unicode(0) ""

-- Iterator 23 --
unicode(6) "tcejbo"

-- Iterator 24 --

Warning: strrev() expects parameter 1 to be string (Unicode or binary), resource given in %s on line %d
NULL

-- Iterator 25 --
unicode(0) ""

-- Iterator 26 --
unicode(0) ""
*** Done ***
