--TEST--
Test decoct() function : usage variations - different data types as $num arg
--INI--
precision=14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
echo "*** Testing decoct() : usage variations ***\n";

$inputs = [
       // int data
/*1*/  0,
       1,
       12345,
       -2345,
       4294967295,  // largest decimal
       4294967296,

       // float data
/* 7*/ 12.3456789000e10,

       // boolean data
/* 8*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*12*/ "",
       '',
];

// loop through each element of $inputs to check the behaviour of decoct()
foreach ($inputs as $i => $input) {
    $iterator = $i + 1;
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump(decoct($input));
    } catch (TypeError $exception) {
        echo $exception->getMessage() . "\n";
    }
}

?>
--EXPECT--
*** Testing decoct() : usage variations ***

-- Iteration 1 --
string(1) "0"

-- Iteration 2 --
string(1) "1"

-- Iteration 3 --
string(5) "30071"

-- Iteration 4 --
string(11) "37777773327"

-- Iteration 5 --
decoct(): Argument #1 ($num) must be of type int, float given

-- Iteration 6 --
decoct(): Argument #1 ($num) must be of type int, float given

-- Iteration 7 --
decoct(): Argument #1 ($num) must be of type int, float given

-- Iteration 8 --
string(1) "1"

-- Iteration 9 --
string(1) "0"

-- Iteration 10 --
string(1) "1"

-- Iteration 11 --
string(1) "0"

-- Iteration 12 --
decoct(): Argument #1 ($num) must be of type int, string given

-- Iteration 13 --
decoct(): Argument #1 ($num) must be of type int, string given
