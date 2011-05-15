--TEST--
Test get_html_translation_table() function : usage variations - unexpected table values
--FILE--
<?php
/* Prototype  : array get_html_translation_table ( [int $table [, int $quote_style [, string charset_hint]]] )
 * Description: Returns the internal translation table used by htmlspecialchars and htmlentities
 * Source code: ext/standard/html.c
*/

/*
 * test get_html_translation_table() with unexpected value for argument $table 
*/

echo "*** Testing get_html_translation_table() : usage variations ***\n";
// initialize all required variables
$quote_style = ENT_COMPAT;

// get an unset variable
$unset_var = 10;
unset($unset_var);

// a resource variable 
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
// when $table arugment is supplied with different values
echo "\n--- Testing get_html_translation_table() by supplying different values for 'table' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $table = $values [$index];

  $v = get_html_translation_table($table, ENT_COMPAT, "UTF-8");
  if (is_array($v) && count($v) > 100)
    var_dump(count($v));
   elseif (is_array($v)) {
    asort($v);
    var_dump($v);
   } else {
    var_dump($v);
   }
   
  $v = get_html_translation_table($table, $quote_style, "UTF-8");
  if (is_array($v) && count($v) > 100)
    var_dump(count($v));
   elseif (is_array($v)) {
    asort($v);
    var_dump($v);
   } else {
    var_dump($v);
   }

  $counter ++;
}

// close resource
fclose($fp);

echo "Done\n";
?>
--EXPECTF--
*** Testing get_html_translation_table() : usage variations ***

--- Testing get_html_translation_table() by supplying different values for 'table' argument ---
-- Iteration 1 --

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %d
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %d
NULL
-- Iteration 2 --

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %d
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %d
NULL
-- Iteration 3 --

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %d
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %d
NULL
-- Iteration 4 --

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %d
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %d
NULL
-- Iteration 5 --

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %d
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %d
NULL
-- Iteration 6 --
int(252)
int(252)
-- Iteration 7 --
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
-- Iteration 8 --
int(252)
int(252)
-- Iteration 9 --
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
-- Iteration 10 --

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %d
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %d
NULL
-- Iteration 11 --

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %d
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %d
NULL
-- Iteration 12 --

Warning: get_html_translation_table() expects parameter 1 to be long, object given in %s on line %d
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, object given in %s on line %d
NULL
-- Iteration 13 --

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %d
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %d
NULL
-- Iteration 14 --

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %d
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %d
NULL
-- Iteration 15 --
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
-- Iteration 16 --
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
-- Iteration 17 --

Warning: get_html_translation_table() expects parameter 1 to be long, resource given in %s on line %d
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, resource given in %s on line %d
NULL
-- Iteration 18 --
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
-- Iteration 19 --
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
Done
