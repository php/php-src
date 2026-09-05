--TEST--
Test printf() function : error conditions
--FILE--
<?php
echo "*** Testing printf() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing printf() function with Zero arguments --\n";
try {
    var_dump( printf() );
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n-- Testing printf() function with less than expected no. of arguments --\n";
$format1 = '%s';
$format2 = '%s%s';
$format3 = '%s%s%s';
$arg1 = 'one';
$arg2 = 'two';

echo "\n-- Call printf with one argument less than expected --\n";
try {
    var_dump( printf($format1) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump( printf($format2,$arg1) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump( printf($format3,$arg1,$arg2) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n-- Call printf with two argument less than expected --\n";
try {
    var_dump( printf($format2) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump( printf($format3,$arg1) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n-- Call printf with three argument less than expected --\n";
try {
    var_dump( printf($format3) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing printf() : error conditions ***

-- Testing printf() function with Zero arguments --
ArgumentCountError: printf() expects at least 1 argument, 0 given

-- Testing printf() function with less than expected no. of arguments --

-- Call printf with one argument less than expected --
ArgumentCountError: 2 arguments are required, 1 given
ArgumentCountError: 3 arguments are required, 2 given
ArgumentCountError: 4 arguments are required, 3 given

-- Call printf with two argument less than expected --
ArgumentCountError: 3 arguments are required, 1 given
ArgumentCountError: 4 arguments are required, 2 given

-- Call printf with three argument less than expected --
ArgumentCountError: 4 arguments are required, 1 given
