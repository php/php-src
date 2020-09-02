--TEST--
Test sprintf() function : error conditions
--FILE--
<?php
echo "*** Testing sprintf() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing sprintf() function with Zero arguments --\n";
try {
    var_dump( sprintf() );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "\n-- Testing sprintf() function with less than expected no. of arguments --\n";
$format1 = '%s';
$format2 = '%s%s';
$format3 = '%s%s%s';
$arg1 = 'one';
$arg2 = 'two';

// with one argument less than expected
try {
    var_dump( sprintf($format1) );
} catch (\ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( sprintf($format2,$arg1) );
} catch (\ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( sprintf($format3,$arg1,$arg2) );
} catch (\ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

// with two argument less than expected
try {
    var_dump( sprintf($format2) );
} catch (\ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( sprintf($format3,$arg1) );
} catch (\ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

// with three argument less than expected
try {
    var_dump( sprintf($format3) );
} catch (\ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(sprintf('%100$d %d'));
} catch (\ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(sprintf("foo %", 42));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : error conditions ***

-- Testing sprintf() function with Zero arguments --
sprintf() expects at least %d parameter, %d given

-- Testing sprintf() function with less than expected no. of arguments --
2 parameters are required, 1 given
3 parameters are required, 2 given
4 parameters are required, 3 given
3 parameters are required, 1 given
4 parameters are required, 2 given
4 parameters are required, 1 given
101 parameters are required, 1 given
Missing format specifier at end of string
Done
