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
fclose($fp);

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
string(2) "12"

-- Iteration 8 --
string(11) "37777777766"

-- Iteration 9 --
decoct(): Argument #1 ($num) must be of type int, float given

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
decoct(): Argument #1 ($num) must be of type int, string given

-- Iteration 19 --
decoct(): Argument #1 ($num) must be of type int, string given

-- Iteration 20 --
decoct(): Argument #1 ($num) must be of type int, array given

-- Iteration 21 --
decoct(): Argument #1 ($num) must be of type int, string given

-- Iteration 22 --
decoct(): Argument #1 ($num) must be of type int, string given

-- Iteration 23 --
decoct(): Argument #1 ($num) must be of type int, string given

-- Iteration 24 --
decoct(): Argument #1 ($num) must be of type int, classA given

-- Iteration 25 --
string(1) "0"

-- Iteration 26 --
string(1) "0"

-- Iteration 27 --
decoct(): Argument #1 ($num) must be of type int, resource given
