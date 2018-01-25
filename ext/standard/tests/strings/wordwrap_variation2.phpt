--TEST--
Test wordwrap() function : usage variations  - unexpected values for width argument
--FILE--
<?php
/* Prototype  : string wordwrap ( string $str [, int $width [, string $break [, bool $cut]]] )
 * Description: Wraps buffer to selected number of characters using string break char
 * Source code: ext/standard/string.c
*/

/*
 * test wordwrap by passing different values for width argument
*/
echo "*** Testing wordwrap() : usage variations ***\n";
// initialize all required variables
$str = 'testing wordwrap function';
$break = '<br />\n';
$cut = true;

// resource var
$fp = fopen(__FILE__, "r");

// get an unset variable
$unset_var = 10;
unset($unset_var);


// array with different values as width
$values =  array (
  // zerovalue for width
  0,

  // -ve value for width
  -1,
  -10,

  // array values
  array(),
  array(0),
  array(1),
  array(1, 2),
  array('color' => 'red', 'item' => 'pen'),

  // boolean values
  true,
  false,
  TRUE,
  FALSE,

  // string values
  "string",
  'string',

  // objects
  new stdclass(),

  // Null value
  NULL,
  null,

  // empty string
  "",
  '',

  // resource variable
  $fp,

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);


// loop though each element of the array and check the working of wordwrap()
// when $width argument is supplied with different values
echo "\n--- Testing wordwrap() by supplying different values for 'width' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $width = $values [$index];

  var_dump( wordwrap($str, $width) );
  var_dump( wordwrap($str, $width, $break) );

  // cut as false
  $cut = false;
  var_dump( wordwrap($str, $width, $break, $cut) );

  // cut as true
  $cut = true;
  var_dump( wordwrap($str, $width, $break, $cut) );

  $counter ++;
}

// close the resource
fclose($fp);

echo "Done\n";
?>
--EXPECTF--
*** Testing wordwrap() : usage variations ***

--- Testing wordwrap() by supplying different values for 'width' argument ---
-- Iteration 1 --
string(25) "testing
wordwrap
function"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(39) "testing<br />\nwordwrap<br />\nfunction"

Warning: wordwrap(): Can't force cut when width is zero in %s on line %d
bool(false)
-- Iteration 2 --
string(25) "testing
wordwrap
function"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(223) "<br />\nt<br />\ne<br />\ns<br />\nt<br />\ni<br />\nn<br />\ng<br />\n<br />\nw<br />\no<br />\nr<br />\nd<br />\nw<br />\nr<br />\na<br />\np<br />\n<br />\nf<br />\nu<br />\nn<br />\nc<br />\nt<br />\ni<br />\no<br />\nn"
-- Iteration 3 --
string(25) "testing
wordwrap
function"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(223) "<br />\nt<br />\ne<br />\ns<br />\nt<br />\ni<br />\nn<br />\ng<br />\n<br />\nw<br />\no<br />\nr<br />\nd<br />\nw<br />\nr<br />\na<br />\np<br />\n<br />\nf<br />\nu<br />\nn<br />\nc<br />\nt<br />\ni<br />\no<br />\nn"
-- Iteration 4 --

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL
-- Iteration 5 --

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL
-- Iteration 6 --

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL
-- Iteration 7 --

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL
-- Iteration 8 --

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, array given in %s on line %d
NULL
-- Iteration 9 --
string(25) "testing
wordwrap
function"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(199) "t<br />\ne<br />\ns<br />\nt<br />\ni<br />\nn<br />\ng<br />\nw<br />\no<br />\nr<br />\nd<br />\nw<br />\nr<br />\na<br />\np<br />\nf<br />\nu<br />\nn<br />\nc<br />\nt<br />\ni<br />\no<br />\nn"
-- Iteration 10 --
string(25) "testing
wordwrap
function"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(39) "testing<br />\nwordwrap<br />\nfunction"

Warning: wordwrap(): Can't force cut when width is zero in %s on line %d
bool(false)
-- Iteration 11 --
string(25) "testing
wordwrap
function"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(199) "t<br />\ne<br />\ns<br />\nt<br />\ni<br />\nn<br />\ng<br />\nw<br />\no<br />\nr<br />\nd<br />\nw<br />\nr<br />\na<br />\np<br />\nf<br />\nu<br />\nn<br />\nc<br />\nt<br />\ni<br />\no<br />\nn"
-- Iteration 12 --
string(25) "testing
wordwrap
function"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(39) "testing<br />\nwordwrap<br />\nfunction"

Warning: wordwrap(): Can't force cut when width is zero in %s on line %d
bool(false)
-- Iteration 13 --

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL
-- Iteration 14 --

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL
-- Iteration 15 --

Warning: wordwrap() expects parameter 2 to be integer, object given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, object given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, object given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, object given in %s on line %d
NULL
-- Iteration 16 --
string(25) "testing
wordwrap
function"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(39) "testing<br />\nwordwrap<br />\nfunction"

Warning: wordwrap(): Can't force cut when width is zero in %s on line %d
bool(false)
-- Iteration 17 --
string(25) "testing
wordwrap
function"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(39) "testing<br />\nwordwrap<br />\nfunction"

Warning: wordwrap(): Can't force cut when width is zero in %s on line %d
bool(false)
-- Iteration 18 --

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL
-- Iteration 19 --

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, string given in %s on line %d
NULL
-- Iteration 20 --

Warning: wordwrap() expects parameter 2 to be integer, resource given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, resource given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, resource given in %s on line %d
NULL

Warning: wordwrap() expects parameter 2 to be integer, resource given in %s on line %d
NULL
-- Iteration 21 --
string(25) "testing
wordwrap
function"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(39) "testing<br />\nwordwrap<br />\nfunction"

Warning: wordwrap(): Can't force cut when width is zero in %s on line %d
bool(false)
-- Iteration 22 --
string(25) "testing
wordwrap
function"
string(39) "testing<br />\nwordwrap<br />\nfunction"
string(39) "testing<br />\nwordwrap<br />\nfunction"

Warning: wordwrap(): Can't force cut when width is zero in %s on line %d
bool(false)
Done
