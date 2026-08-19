--TEST--
Test sprintf() function : error conditions
--FILE--
<?php
echo "*** Testing sprintf() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing sprintf() function with Zero arguments --\n";
try {
    var_dump( sprintf() );
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
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
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump( sprintf($format2,$arg1) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump( sprintf($format3,$arg1,$arg2) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// with two argument less than expected
try {
    var_dump( sprintf($format2) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump( sprintf($format3,$arg1) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// with three argument less than expected
try {
    var_dump( sprintf($format3) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(sprintf('%100$d %d'));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(sprintf("foo %", 42));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : error conditions ***

-- Testing sprintf() function with Zero arguments --
ArgumentCountError: sprintf() expects at least 1 argument, 0 given

-- Testing sprintf() function with less than expected no. of arguments --
ArgumentCountError: 2 arguments are required, 1 given
ArgumentCountError: 3 arguments are required, 2 given
ArgumentCountError: 4 arguments are required, 3 given
ArgumentCountError: 3 arguments are required, 1 given
ArgumentCountError: 4 arguments are required, 2 given
ArgumentCountError: 4 arguments are required, 1 given
ArgumentCountError: 101 arguments are required, 1 given
ValueError: Missing format specifier at end of string
Done
