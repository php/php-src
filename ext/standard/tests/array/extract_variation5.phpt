--TEST--
Test extract() function (variation 5) 
--FILE--
<?php

$mixed_array = array(
  array( array("oNe", "tWo", 4), array(10, 20, 30, 40, 50), array() ),
  array( "one" => 1, "one" => 2, "three" => 3, 3, 4, 3 => 33, 4 => 44, 5, 6,
          5.4 => 54, 5.7 => 57, "5.4" => 554, "5.7" => 557 )
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
int(3)
int(0)
int(3)
int(3)
int(0)

-- Iteration 1 --
int(2)
int(2)
int(0)
int(2)
int(8)
int(2)
int(8)
int(8)
int(2)
Done
