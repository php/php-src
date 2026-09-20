--TEST--
Test strtr() function : usage variations - unexpected inputs for 'replace_pairs' argument
--FILE--
<?php
/* Test strtr() function: with unexpected inputs for 'replace_pairs'
 *  and expected type for 'str' arguments
*/

echo "*** Testing strtr() function: with unexpected inputs for 'replace_pairs' ***\n";

//defining 'str' argument
$str = "012atm";

// array of inputs for 'replace_pairs' argument
$replace_pairs_arr =  array (

  // array values
  array(),
  array(0),
  array(1, 2),
);

// loop through with each element of the $replace_pairs array to test strtr() function
$count = 1;
for($index = 0; $index < count($replace_pairs_arr); $index++) {
    echo "\n-- Iteration $count --\n";
    $replace_pairs = $replace_pairs_arr[$index];
    var_dump(strtr($str, $replace_pairs));

    $count ++;
}

echo "*** Done ***";
?>
--EXPECT--
*** Testing strtr() function: with unexpected inputs for 'replace_pairs' ***

-- Iteration 1 --
string(6) "012atm"

-- Iteration 2 --
string(6) "012atm"

-- Iteration 3 --
string(6) "122atm"
*** Done ***
