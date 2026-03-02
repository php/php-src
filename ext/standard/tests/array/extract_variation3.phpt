--TEST--
Test extract() function (variation 3)
--FILE--
<?php

/* various combinations of arrays to be used for the test */
$mixed_array = array(
  array( 6, "six", 7, "seven", 8, "eight", 9, "nine" ),
  array( "a" => "aaa", "A" => "AAA", "c" => "ccc", "d" => "ddd", "e" => "eee" ),
  array( "1" => "one", "2" => "two", "3" => "three", "4" => "four", "5" => "five" ),
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
--EXPECT--
-- Iteration 0 --
int(0)
int(0)
int(0)
int(0)
int(8)
int(0)
int(8)
int(8)
int(0)

-- Iteration 1 --
int(5)
int(5)
int(0)
int(5)
int(5)
int(5)
int(5)
int(5)
int(5)

-- Iteration 2 --
int(0)
int(0)
int(0)
int(0)
int(5)
int(0)
int(5)
int(5)
int(0)
Done
