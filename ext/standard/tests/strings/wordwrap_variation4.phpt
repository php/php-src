--TEST--
Test wordwrap() function : usage variations  - unexptected value for cut argument
--FILE--
<?php
/* Prototype  : string wordwrap ( string $str [, int $width [, string $break [, bool $cut]]] )
 * Description: Wraps buffer to selected number of characters using string break char
 * Source code: ext/standard/string.c
*/

/*
 * test wordwrap() by supplying different values for cut argument
*/

echo "*** Testing wordwrap() : usage variations ***\n";
// initialize all required variables
$str = 'testing wordwrap function';
$width = 10;
$break = '<br />\n';

// get an unset variable
$unset_var = true;
unset($unset_var);

// resource variable
$fp = fopen(__FILE__, "r");

// array with different values
$values =  array (

  // integer values
  0,
  1,
  12345,
  -2345,

  // float values
  10.5,
  -10.5,
  10.5e10,
  10.6E-10,
  .5,

  // array values
  array(),
  array(0),
  array(1),
  array(1, 2),
  array('color' => 'red', 'item' => 'pen'),

  // string values
  "string",
  'string',

  // objects
  new stdclass(),

  // empty string
  "",
  '',

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);

// loop though each element of the array and check the working of wordwrap()
// when $cut argument is supplied with different values
echo "\n--- Testing wordwrap() by supplying different values for 'cut' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $cut = $values [$index];

  var_dump( wordwrap($str, $width, $break, $cut) );

  $counter ++;
}

// close the resource
fclose($fp);

echo "Done\n";
?>
--EXPECTF--
*** Testing wordwrap() : usage variations ***

--- Testing wordwrap() by supplying different values for 'cut' argument ---
-- Iteration 1 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 2 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 3 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 4 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 5 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 6 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 7 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 8 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 9 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 10 --

Warning: wordwrap() expects parameter 4 to be boolean, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: wordwrap() expects parameter 4 to be boolean, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: wordwrap() expects parameter 4 to be boolean, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: wordwrap() expects parameter 4 to be boolean, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: wordwrap() expects parameter 4 to be boolean, array given in %s on line %d
NULL
-- Iteration 15 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 16 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 17 --

Warning: wordwrap() expects parameter 4 to be boolean, object given in %s on line %d
NULL
-- Iteration 18 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 19 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 20 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- Iteration 21 --
string(39) "testing<br />\nwordwrap<br />\nfunction"
Done
