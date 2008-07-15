--TEST--
Test strip_tags() function : obscure values within attributes
--INI--
short_open_tag = on
--FILE--
<?php

echo "*** Testing strip_tags() : obscure functionality ***\n";

// array of arguments 
$string_array = array (
  'hello <img title="<"> world',
  'hello <img title=">"> world',
  'hello <img title=">_<"> world',
  "hello <img title='>_<'> world"
);
  
  		
// Calling strip_tags() with default arguments
// loop through the $string_array to test strip_tags on various inputs
$iteration = 1;
foreach($string_array as $string)
{
  echo "-- Iteration $iteration --\n";
  var_dump( strip_tags($string) );
  $iteration++;
}

echo "Done";
?>
--EXPECTF--
*** Testing strip_tags() : obscure functionality ***
-- Iteration 1 --
string(12) "hello  world"
-- Iteration 2 --
string(12) "hello  world"
-- Iteration 3 --
string(12) "hello  world"
-- Iteration 4 --
string(12) "hello  world"
Done
