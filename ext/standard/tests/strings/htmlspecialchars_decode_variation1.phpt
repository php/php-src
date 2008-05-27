--TEST--
Test htmlspecialchars_decode() function : usage variations - unexpected values for 'string' argument
--FILE--
<?php
/* Prototype  : string htmlspecialchars_decode(string $string [, int $quote_style])
 * Description: Convert special HTML entities back to characters 
 * Source code: ext/standard/html.c
*/

/*
 * testing htmlspecialchars_decode() with unexpected input values for $string argument
*/

echo "*** Testing htmlspecialchars_decode() : usage variations ***\n";

//get a class
class classA 
{
  function __toString() {
    return "ClassAObject";
  }
}

//get a resource variable
$file_handle=fopen(__FILE__, "r");

//get an unset variable
$unset_var = 10;
unset($unset_var);

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
      new classA(),

      // undefined data
      @$undefined_var,

      // unset data
      @$unset_var,

      //resource
      $file_handle
);

// loop through each element of the array for string
$iterator = 1;
foreach($values as $value) {
      echo "-- Iterator $iterator --\n";
      var_dump( htmlspecialchars_decode($value) );
      $iterator++;
};

// close the file resource used
fclose($file_handle);

echo "Done";
?>
--EXPECTF--
*** Testing htmlspecialchars_decode() : usage variations ***
-- Iterator 1 --
unicode(1) "0"
-- Iterator 2 --
unicode(1) "1"
-- Iterator 3 --
unicode(5) "12345"
-- Iterator 4 --
unicode(5) "-2345"
-- Iterator 5 --
unicode(4) "10.5"
-- Iterator 6 --
unicode(5) "-10.5"
-- Iterator 7 --
unicode(12) "105000000000"
-- Iterator 8 --
unicode(7) "1.06E-9"
-- Iterator 9 --
unicode(3) "0.5"
-- Iterator 10 --

Warning: htmlspecialchars_decode() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iterator 11 --

Warning: htmlspecialchars_decode() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iterator 12 --

Warning: htmlspecialchars_decode() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iterator 13 --

Warning: htmlspecialchars_decode() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iterator 14 --

Warning: htmlspecialchars_decode() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
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
unicode(12) "ClassAObject"
-- Iterator 24 --
unicode(0) ""
-- Iterator 25 --
unicode(0) ""
-- Iterator 26 --

Warning: htmlspecialchars_decode() expects parameter 1 to be string (Unicode or binary), resource given in %s on line %d
NULL
Done
