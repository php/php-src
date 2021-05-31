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

// heredoc string
$heredoc = <<<EOT
abc
xyz
EOT;

// get a class
class classA
{
}

// get a resource variable
$fp = fopen(__FILE__, "r");

$inputs = array(
       // int data
/*1*/  0,
       1,
       12345,
       -2345,
       18446744073709551615,  // largest decimal
       18446744073709551616,

       // float data
/*7*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // boolean data
/*14*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*18*/ "",
       '',
       array(),

       // string data
/*21*/ "abcxyz",
       'abcxyz',
       $heredoc,

       // object data
/*24*/ new classA(),

       // resource variable
/*27*/ $fp
);

// loop through each element of $inputs to check the behaviour of decbin()
foreach($inputs as $i => $input) {
    $iterator = $i + 1;
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump(decbin($input));
    } catch (TypeError $exception) {
        echo $exception->getMessage() . "\n";
    }
}
fclose($fp);
?>
--EXPECTF--
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
decbin(): Argument #1 ($num) must be of type int, float given

-- Iteration 6 --
decbin(): Argument #1 ($num) must be of type int, float given

-- Iteration 7 --

Deprecated: Implicit conversion from non-compatible float 10.5 to int in %s on line %d
string(4) "1010"

-- Iteration 8 --

Deprecated: Implicit conversion from non-compatible float -10.5 to int in %s on line %d
string(64) "1111111111111111111111111111111111111111111111111111111111110110"

-- Iteration 9 --
string(37) "1110010111110100110010001101000001000"

-- Iteration 10 --

Deprecated: Implicit conversion from non-compatible float 1.23456789E-9 to int in %s on line %d
string(1) "0"

-- Iteration 11 --

Deprecated: Implicit conversion from non-compatible float 0.5 to int in %s on line %d
string(1) "0"

-- Iteration 12 --
string(1) "1"

-- Iteration 13 --
string(1) "0"

-- Iteration 14 --
string(1) "1"

-- Iteration 15 --
string(1) "0"

-- Iteration 16 --
decbin(): Argument #1 ($num) must be of type int, string given

-- Iteration 17 --
decbin(): Argument #1 ($num) must be of type int, string given

-- Iteration 18 --
decbin(): Argument #1 ($num) must be of type int, array given

-- Iteration 19 --
decbin(): Argument #1 ($num) must be of type int, string given

-- Iteration 20 --
decbin(): Argument #1 ($num) must be of type int, string given

-- Iteration 21 --
decbin(): Argument #1 ($num) must be of type int, string given

-- Iteration 22 --
decbin(): Argument #1 ($num) must be of type int, classA given

-- Iteration 23 --
decbin(): Argument #1 ($num) must be of type int, resource given
