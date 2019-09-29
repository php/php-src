--TEST--
Test range() function (errors)
--INI--
precision=14
--FILE--
<?php

echo "\n*** Testing error conditions ***\n";

echo "\n-- Testing ( (low < high) && (step = 0) ) --\n";
try {
    var_dump( range(1, 2, 0) );
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump( range("a", "b", 0) );
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

echo "\n\n-- Testing ( (low > high) && (step = 0) ) --\n";
try {
    var_dump( range(2, 1, 0) );
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump( range("b", "a", 0) );
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

echo "\n\n-- Testing ( (low < high) && (high-low < step) ) --\n";
try {
    var_dump( range(1.0, 7.0, 6.5) );
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

echo "\n\n-- Testing ( (low > high) && (low-high < step) ) --\n";
try {
    var_dump( range(7.0, 1.0, 6.5) );
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

echo "\n-- Testing other conditions --\n";
try {
    var_dump( range(-1, -2, 2) );
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump( range("a", "j", "z") );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump( range(0, 1, "140962482048819216326.24") );
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump( range(0, 1, "140962482048819216326.24.") );
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

echo "\n-- Testing Invalid steps --\n";
$step_arr = array( "string", NULL, FALSE, "", "\0" );

foreach( $step_arr as $step ) {
    try {
        var_dump( range( 1, 5, $step ) );
    } catch (\TypeError $e) {
        echo $e->getMessage(), "\n";
    } catch (\Error $e) {
        echo $e->getMessage(), "\n";
    }
}

echo "Done\n";
?>
--EXPECTF--
*** Testing error conditions ***

-- Testing ( (low < high) && (step = 0) ) --
step exceeds the specified range
step exceeds the specified range


-- Testing ( (low > high) && (step = 0) ) --
step exceeds the specified range
step exceeds the specified range


-- Testing ( (low < high) && (high-low < step) ) --
step exceeds the specified range


-- Testing ( (low > high) && (low-high < step) ) --
step exceeds the specified range

-- Testing other conditions --
step exceeds the specified range
range() expects parameter 3 to be int or float, string given
step exceeds the specified range

Notice: A non well formed numeric value encountered in %s on line %d
step exceeds the specified range

-- Testing Invalid steps --
range() expects parameter 3 to be int or float, string given
step exceeds the specified range
step exceeds the specified range
range() expects parameter 3 to be int or float, string given
range() expects parameter 3 to be int or float, string given
Done
