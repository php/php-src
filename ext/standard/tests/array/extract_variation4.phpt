--TEST--
Test extract() function (variation 4)
--FILE--
<?php

$mixed_array = array(
  array( 1 => "one", 2 => "two", 3 => 7, 4 => "four", 5 => "five" ),
  array( "f" => "fff", "1" => "one", 4 => 6, "" => "blank", 2.4 => "float", "F" => "FFF",
         "blank" => "", 3.7 => 3.7, 5.4 => 7, 6 => 8.6, '5' => "Five", "4name" => "jonny", "a" => NULL, NULL => 3 ),
  array( 12, "name", 'age', '45' ),
);

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

echo "Done\n";
?>
--EXPECTF--
-- Iteration 0 --
int(0)
int(0)
int(0)
int(0)
int(5)
int(0)
int(5)
int(5)
int(0)

-- Iteration 1 --
int(4)
int(4)
int(0)
int(4)
int(12)
int(4)
int(11)
int(11)
int(4)

-- Iteration 2 --
int(0)
int(0)
int(0)
int(0)
int(4)
int(0)
int(4)
int(4)
int(0)
Done
