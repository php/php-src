--TEST--
Test decbin() function : usage variations - different data types as $number arg
--INI--
precision=14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
echo "*** Testing decbin() : usage variations ***\n";

$inputs = [
       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/* 7*/ 12.3456789000e10,

       // boolean data
/* 8*/ true,
       false,
       TRUE,
       FALSE,
];

// loop through each element of $inputs to check the behaviour of decbin()
foreach ($inputs as $i => $input) {
    $iterator = $i + 1;
    echo "\n-- Iteration $iterator --\n";
    var_dump(decbin($input));
}

?>
--EXPECT--
*** Testing decbin() : usage variations ***

-- Iteration 1 --
string(1) "0"

-- Iteration 2 --
string(1) "1"

-- Iteration 3 --
string(14) "11000000111001"

-- Iteration 4 --
string(64) "1111111111111111111111111111111111111111111111111111011011010111"

-- Iteration 5 --
string(37) "1110010111110100110010001101000001000"

-- Iteration 6 --
string(1) "1"

-- Iteration 7 --
string(1) "0"

-- Iteration 8 --
string(1) "1"

-- Iteration 9 --
string(1) "0"
