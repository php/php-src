--TEST--
Test array_intersect_uassoc() function : usage variation - arrays containing referenced variables
--FILE--
<?php
echo "*** Testing array_intersect_uassoc() : usage variation ***\n";

//Initialize variables
$ref_var = 'a';
$array1 = array('a', $ref_var);
$array2 = array('a' => 1, &$ref_var);

echo "\n-- Testing array_intersect_uassoc() function with referenced variable \$ref_var has value '$ref_var' --\n";
var_dump( array_intersect_uassoc($array1, $array2, "strcasecmp") );

// re-assign reference variable to different value
$ref_var = 10;
echo "\n-- Testing array_intersect_uassoc() function with referenced variable \$ref_var value changed to $ref_var --\n";
var_dump( array_intersect_uassoc($array1, $array2, "strcasecmp") );

//When array are referenced
$array2 = &$array1;
echo "\n-- Testing array_intersect_uassoc() function when \$array2 is referencd to \$array1 --\n";
var_dump( array_intersect_uassoc($array1, $array2, "strcasecmp") );
?>
--EXPECT--
*** Testing array_intersect_uassoc() : usage variation ***

-- Testing array_intersect_uassoc() function with referenced variable $ref_var has value 'a' --
array(1) {
  [0]=>
  string(1) "a"
}

-- Testing array_intersect_uassoc() function with referenced variable $ref_var value changed to 10 --
array(0) {
}

-- Testing array_intersect_uassoc() function when $array2 is referencd to $array1 --
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "a"
}
