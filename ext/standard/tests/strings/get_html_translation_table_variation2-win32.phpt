--TEST--
Test get_html_translation_table() function : usage variations - unexpected quote_style values
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != "WIN"){  
  die('skip only for Windows');
}

if( !setlocale(LC_ALL, "English_United States.1252") ) {
  die('skip failed to set locale settings to "English_United States.1252"');
}

?>
--FILE--
<?php
/* Prototype  : array get_html_translation_table ( [int $table [, int $quote_style]] )
 * Description: Returns the internal translation table used by htmlspecialchars and htmlentities
 * Source code: ext/standard/html.c
*/

/*
 * test get_html_translation_table() with unexpteced value for argument $quote_style
*/

//set locale
setlocale(LC_ALL, "English_United States.1252"); 

echo "*** Testing get_html_translation_table() : usage variations ***\n";
// initialize all required variables
$table = HTML_SPECIALCHARS;

// get an unset variable
$unset_var = 10;
unset($unset_var);

// a resource var
$fp = fopen(__FILE__, "r");

// array with different values
$values =  array (

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

  // empty string
  "",
  '',

  // null vlaues
  NULL,
  null,

  // resource var
  $fp, 

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);


// loop through each element of the array and check the working of get_html_translation_table()
// when $quote_style arugment is supplied with different values
echo "\n--- Testing get_html_translation_table() by supplying different values for 'quote_style' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $quote_style = $values [$index];

  var_dump( get_html_translation_table($table, $quote_style) );

  $counter ++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing get_html_translation_table() : usage variations ***

--- Testing get_html_translation_table() by supplying different values for 'quote_style' argument ---
-- Iteration 1 --

Warning: get_html_translation_table() expects parameter 2 to be long, array given in %s on line %s
NULL
-- Iteration 2 --

Warning: get_html_translation_table() expects parameter 2 to be long, array given in %s on line %s
NULL
-- Iteration 3 --

Warning: get_html_translation_table() expects parameter 2 to be long, array given in %s on line %s
NULL
-- Iteration 4 --

Warning: get_html_translation_table() expects parameter 2 to be long, array given in %s on line %s
NULL
-- Iteration 5 --

Warning: get_html_translation_table() expects parameter 2 to be long, array given in %s on line %s
NULL
-- Iteration 6 --
array(4) {
  ["'"]=>
  string(5) "&#39;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 7 --
array(3) {
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 8 --
array(4) {
  ["'"]=>
  string(5) "&#39;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 9 --
array(3) {
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 10 --

Warning: get_html_translation_table() expects parameter 2 to be long, string given in %s on line %s
NULL
-- Iteration 11 --

Warning: get_html_translation_table() expects parameter 2 to be long, string given in %s on line %s
NULL
-- Iteration 12 --

Warning: get_html_translation_table() expects parameter 2 to be long, object given in %s on line %s
NULL
-- Iteration 13 --

Warning: get_html_translation_table() expects parameter 2 to be long, string given in %s on line %s
NULL
-- Iteration 14 --

Warning: get_html_translation_table() expects parameter 2 to be long, string given in %s on line %s
NULL
-- Iteration 15 --
array(3) {
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 16 --
array(3) {
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 17 --

Warning: get_html_translation_table() expects parameter 2 to be long, resource given in %s on line %s
NULL
-- Iteration 18 --
array(3) {
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 19 --
array(3) {
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
Done
