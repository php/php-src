--TEST--
Test var_export() function with valid boolean values
--FILE--
<?php

echo "*** Testing var_export() with valid boolean values ***\n";
// different valid  boolean values
$valid_bool = array(
            "1" => 1,
            "TRUE" => TRUE,
            "true" => true,
            "0" => 0,
            "FALSE" => FALSE,
            "false" => false
);

/* Loop to check for above boolean values with var_export() */
echo "\n*** Output for boolean values ***\n";
foreach($valid_bool as $key => $bool_value) {
    echo "\n-- Iteration: $key --\n";
    var_export( $bool_value );
    echo "\n";
    var_export( $bool_value, FALSE);
    echo "\n";
    var_dump( var_export( $bool_value, TRUE) );
    echo "\n";
}
?>
--EXPECT--
*** Testing var_export() with valid boolean values ***

*** Output for boolean values ***

-- Iteration: 1 --
1
1
string(1) "1"


-- Iteration: TRUE --
true
true
string(4) "true"


-- Iteration: true --
true
true
string(4) "true"


-- Iteration: 0 --
0
0
string(1) "0"


-- Iteration: FALSE --
false
false
string(5) "false"


-- Iteration: false --
false
false
string(5) "false"

