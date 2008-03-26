--TEST--
Test strip_tags() function : usage variations - unexpected values for 'allowable_tags'
--INI--
short_open_tag = on
--FILE--
<?php
/* Prototype  : string strip_tags(string $str [, string $allowable_tags])
 * Description: Strips HTML and PHP tags from a string 
 * Source code: ext/standard/string.c
*/

/*
 * testing functionality of strip_tags() by giving unexpected values for $allowable_tags argument
*/

echo "*** Testing strip_tags() : usage variations ***\n";

// Initialise function argument
$string = "<html><a>hello</a></html><p>world</p><!-- COMMENT --><?php echo hello ?>";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//get a resource variable
$fp = fopen(__FILE__, "r");

//get a class
class classA{
   public function __toString(){
     return "Class A Object";
   }
}

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

      // resource variable
      $fp
);

// loop through each element of the array for allowable_tags
$iterator = 1;
foreach($values as $value) {
      echo "-- Iteration $iterator --\n";
      var_dump( strip_tags($string, $value) );
      $iterator++;
};

echo "Done";
?>
--EXPECTF--
*** Testing strip_tags() : usage variations ***
-- Iteration 1 --
string(10) "helloworld"
-- Iteration 2 --
string(10) "helloworld"
-- Iteration 3 --
string(10) "helloworld"
-- Iteration 4 --
string(10) "helloworld"
-- Iteration 5 --
string(10) "helloworld"
-- Iteration 6 --
string(10) "helloworld"
-- Iteration 7 --
string(10) "helloworld"
-- Iteration 8 --
string(10) "helloworld"
-- Iteration 9 --
string(10) "helloworld"
-- Iteration 10 --

Warning: strip_tags() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 11 --

Warning: strip_tags() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 12 --

Warning: strip_tags() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 13 --

Warning: strip_tags() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 14 --

Warning: strip_tags() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 15 --
string(10) "helloworld"
-- Iteration 16 --
string(10) "helloworld"
-- Iteration 17 --
string(10) "helloworld"
-- Iteration 18 --
string(10) "helloworld"
-- Iteration 19 --
string(10) "helloworld"
-- Iteration 20 --
string(10) "helloworld"
-- Iteration 21 --
string(10) "helloworld"
-- Iteration 22 --
string(10) "helloworld"
-- Iteration 23 --
string(10) "helloworld"
-- Iteration 24 --
string(10) "helloworld"
-- Iteration 25 --
string(10) "helloworld"
-- Iteration 26 --

Warning: strip_tags() expects parameter 2 to be string (Unicode or binary), resource given in %s on line %d
NULL
Done
--UEXPECTF--
*** Testing strip_tags() : usage variations ***
-- Iteration 1 --
unicode(10) "helloworld"
-- Iteration 2 --
unicode(10) "helloworld"
-- Iteration 3 --
unicode(10) "helloworld"
-- Iteration 4 --
unicode(10) "helloworld"
-- Iteration 5 --
unicode(10) "helloworld"
-- Iteration 6 --
unicode(10) "helloworld"
-- Iteration 7 --
unicode(10) "helloworld"
-- Iteration 8 --
unicode(10) "helloworld"
-- Iteration 9 --
unicode(10) "helloworld"
-- Iteration 10 --

Warning: strip_tags() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 11 --

Warning: strip_tags() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 12 --

Warning: strip_tags() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 13 --

Warning: strip_tags() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 14 --

Warning: strip_tags() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 15 --
unicode(10) "helloworld"
-- Iteration 16 --
unicode(10) "helloworld"
-- Iteration 17 --
unicode(10) "helloworld"
-- Iteration 18 --
unicode(10) "helloworld"
-- Iteration 19 --
unicode(10) "helloworld"
-- Iteration 20 --
unicode(10) "helloworld"
-- Iteration 21 --
unicode(10) "helloworld"
-- Iteration 22 --
unicode(10) "helloworld"
-- Iteration 23 --
unicode(10) "helloworld"
-- Iteration 24 --
unicode(10) "helloworld"
-- Iteration 25 --
unicode(10) "helloworld"
-- Iteration 26 --

Warning: strip_tags() expects parameter 2 to be string (Unicode or binary), resource given in %s on line %d
NULL
Done
