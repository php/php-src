--TEST--
Test decbin() function : usage variations - different data types as $num arg
--INI--
precision=14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
echo "*** Testing decbin() : usage variations ***\n";
//get an unset variable
$unset_var = 10;
unset ($unset_var);

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
       4294967295,  // largest decimal
       4294967296,

       // float data
/*7*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
/*12*/ NULL,
       null,

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

       // undefined data
/*25*/ @$undefined_var,

       // unset data
/*26*/ @$unset_var,

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
--EXPECT--
*** Testing decbin() : usage variations ***

-- Iteration 1 --
string(1) "0"

-- Iteration 2 --
string(1) "1"

-- Iteration 3 --
string(14) "11000000111001"

-- Iteration 4 --
string(32) "11111111111111111111011011010111"

-- Iteration 5 --
decbin(): Argument #1 ($num) must be of type int, float given

-- Iteration 6 --
decbin(): Argument #1 ($num) must be of type int, float given

-- Iteration 7 --
string(4) "1010"

-- Iteration 8 --
string(32) "11111111111111111111111111110110"

-- Iteration 9 --
decbin(): Argument #1 ($num) must be of type int, float given

-- Iteration 10 --
string(1) "0"

-- Iteration 11 --
string(1) "0"

-- Iteration 12 --
string(1) "0"

-- Iteration 13 --
string(1) "0"

-- Iteration 14 --
string(1) "1"

-- Iteration 15 --
string(1) "0"

-- Iteration 16 --
string(1) "1"

-- Iteration 17 --
string(1) "0"

-- Iteration 18 --
decbin(): Argument #1 ($num) must be of type int, string given

-- Iteration 19 --
decbin(): Argument #1 ($num) must be of type int, string given

-- Iteration 20 --
decbin(): Argument #1 ($num) must be of type int, array given

-- Iteration 21 --
decbin(): Argument #1 ($num) must be of type int, string given

-- Iteration 22 --
decbin(): Argument #1 ($num) must be of type int, string given

-- Iteration 23 --
decbin(): Argument #1 ($num) must be of type int, string given

-- Iteration 24 --
decbin(): Argument #1 ($num) must be of type int, classA given

-- Iteration 25 --
string(1) "0"

-- Iteration 26 --
string(1) "0"

-- Iteration 27 --
decbin(): Argument #1 ($num) must be of type int, resource given
