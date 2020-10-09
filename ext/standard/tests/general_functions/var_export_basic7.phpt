--TEST--
Test var_export() function with valid null values
--FILE--
<?php
echo "*** Testing var_export() with valid null values ***\n";

// different valid  null values
$unset_var = array();
unset ($unset_var); // now a null
$null_var = NULL;

$valid_nulls = array(
                "NULL" =>  NULL,
                "null" => null,
                "null_var" => $null_var,
);

/* Loop to check for above null values with var_export() */
echo "\n*** Output for null values ***\n";
foreach($valid_nulls as $key => $null_value) {
    echo "\n-- Iteration: $key --\n";
    var_export( $null_value );
    echo "\n";
    var_export( $null_value, FALSE);
    echo "\n";
    var_dump( var_export( $null_value, true) );
    echo "\n";
}
?>
--EXPECT--
*** Testing var_export() with valid null values ***

*** Output for null values ***

-- Iteration: NULL --
NULL
NULL
string(4) "NULL"


-- Iteration: null --
NULL
NULL
string(4) "NULL"


-- Iteration: null_var --
NULL
NULL
string(4) "NULL"

