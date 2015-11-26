--TEST--
Test is_string() function
--FILE--
<?php
/* Prototype: bool is_string ( mixed $var );
 * Description: Finds whether the given variable is a string
 */

echo "*** Testing is_string() with valid string values ***\n";
// different valid strings 

/* string created using Heredoc (<<<) */
$heredoc_string = <<<EOT
This is string defined
using heredoc.
EOT;
/* heredoc string with only numerics */
$heredoc_numeric_string = <<<EOT
123456 3993
4849 string 
EOT;
/* null heardoc string */
$heredoc_empty_string = <<<EOT
EOT;
$heredoc_null_string = <<<EOT
NULL
EOT;

$strings = array(
  "",
  " ",
  '',
  ' ',
  "string",
  'string',
  "NULL",
  'null',
  "FALSE",
  'true',
  "\x0b",
  "\0",
  '\0',
  '\060',
  "\070",
  "0x55F",
  "055",
  "@#$#$%%$^^$%^%^$^&",
  $heredoc_string,
  $heredoc_numeric_string,
  $heredoc_empty_string,
  $heredoc_null_string
);
/* loop to check that is_string() recognizes different 
   strings, expected output bool(true) */
$loop_counter = 1;
foreach ($strings as $string ) {
  echo "-- Iteration $loop_counter --\n"; $loop_counter++;
  var_dump( is_string($string) );
}

echo "\n*** Testing is_string() on non string values ***\n";

// get a resource type variable
$fp = fopen (__FILE__, "r");
$dfp = opendir ( dirname(__FILE__) );

// unset vars
$unset_string1 = "string";
$unset_string2 = 'string';
$unset_heredoc = <<<EOT
this is heredoc string
EOT;
// unset the vars 
unset($unset_string1, $unset_string2, $unset_heredoc);

// other types in a array 
$not_strings = array (
  /* integers */
  0,
  1,
  -1,
  -0,
  543915,
  -5322,
  0x0,
  0x1,
  0x55F,
  -0xCCF,
  0123,
  -0654,
  00,
  01,

  /* floats */
  0.0,
  1.0,
  -1.0,
  10.0000000000000000005,
  .5e6,
  -.5E7,
  .5E+8,
  -.5e+90,
  1e5,
  -1e5,
  1E5,
  -1E7,

  /* objects */
  new stdclass,

  /* resources */
  $fp,
  $dfp,

  /* arrays */
  array(),
  array(0),
  array(1),
  array(NULL),
  array(null),
  array("string"),
  array(true),
  array(TRUE),
  array(false),
  array(FALSE),
  array(1,2,3,4),
  array(1 => "One", "two" => 2),

  /* undefined and unset vars */
  @$unset_string1,
  @$unset_string2,
  @$unset_heredoc,
  @$undefined_var 
);
/* loop through the $not_strings to see working of 
   is_string() on non string types, expected output bool(false) */
$loop_counter = 1;
foreach ($not_strings as $type ) {
  echo "-- Iteration $loop_counter --\n"; $loop_counter++;
  var_dump( is_string($type) );
}

echo "\n*** Testing error conditions ***\n";
//Zero argument
var_dump( is_string() );

//arguments more than expected 
var_dump( is_string("string", "test") );
 
echo "Done\n";

// close the resources used
fclose($fp);
closedir($dfp);

?>
--EXPECTF--
*** Testing is_string() with valid string values ***
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)
-- Iteration 5 --
bool(true)
-- Iteration 6 --
bool(true)
-- Iteration 7 --
bool(true)
-- Iteration 8 --
bool(true)
-- Iteration 9 --
bool(true)
-- Iteration 10 --
bool(true)
-- Iteration 11 --
bool(true)
-- Iteration 12 --
bool(true)
-- Iteration 13 --
bool(true)
-- Iteration 14 --
bool(true)
-- Iteration 15 --
bool(true)
-- Iteration 16 --
bool(true)
-- Iteration 17 --
bool(true)
-- Iteration 18 --
bool(true)
-- Iteration 19 --
bool(true)
-- Iteration 20 --
bool(true)
-- Iteration 21 --
bool(true)
-- Iteration 22 --
bool(true)

*** Testing is_string() on non string values ***
-- Iteration 1 --
bool(false)
-- Iteration 2 --
bool(false)
-- Iteration 3 --
bool(false)
-- Iteration 4 --
bool(false)
-- Iteration 5 --
bool(false)
-- Iteration 6 --
bool(false)
-- Iteration 7 --
bool(false)
-- Iteration 8 --
bool(false)
-- Iteration 9 --
bool(false)
-- Iteration 10 --
bool(false)
-- Iteration 11 --
bool(false)
-- Iteration 12 --
bool(false)
-- Iteration 13 --
bool(false)
-- Iteration 14 --
bool(false)
-- Iteration 15 --
bool(false)
-- Iteration 16 --
bool(false)
-- Iteration 17 --
bool(false)
-- Iteration 18 --
bool(false)
-- Iteration 19 --
bool(false)
-- Iteration 20 --
bool(false)
-- Iteration 21 --
bool(false)
-- Iteration 22 --
bool(false)
-- Iteration 23 --
bool(false)
-- Iteration 24 --
bool(false)
-- Iteration 25 --
bool(false)
-- Iteration 26 --
bool(false)
-- Iteration 27 --
bool(false)
-- Iteration 28 --
bool(false)
-- Iteration 29 --
bool(false)
-- Iteration 30 --
bool(false)
-- Iteration 31 --
bool(false)
-- Iteration 32 --
bool(false)
-- Iteration 33 --
bool(false)
-- Iteration 34 --
bool(false)
-- Iteration 35 --
bool(false)
-- Iteration 36 --
bool(false)
-- Iteration 37 --
bool(false)
-- Iteration 38 --
bool(false)
-- Iteration 39 --
bool(false)
-- Iteration 40 --
bool(false)
-- Iteration 41 --
bool(false)
-- Iteration 42 --
bool(false)
-- Iteration 43 --
bool(false)
-- Iteration 44 --
bool(false)
-- Iteration 45 --
bool(false)

*** Testing error conditions ***

Warning: is_string() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

Warning: is_string() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)
Done
