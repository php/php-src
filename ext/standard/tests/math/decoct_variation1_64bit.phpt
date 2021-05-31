--TEST--
Test decoct() function : usage variations - different data types as $number arg
--INI--
precision=14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
echo "*** Testing decoct() : usage variations ***\n";

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

// loop through each element of $inputs to check the behaviour of decoct()
foreach($inputs as $i => $input) {
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
--EXPECTF--
*** Testing decoct() : usage variations ***

-- Iteration 1 --
string(1) "0"

-- Iteration 2 --
string(1) "1"

-- Iteration 3 --
string(5) "30071"

-- Iteration 4 --
string(22) "1777777777777777773327"

-- Iteration 5 --
decoct(): Argument #1 ($num) must be of type int, float given

-- Iteration 6 --
decoct(): Argument #1 ($num) must be of type int, float given

-- Iteration 7 --

Deprecated: Implicit conversion to int from non-compatible float 10.500000 in %s on line %d
string(2) "12"

-- Iteration 8 --

Deprecated: Implicit conversion to int from non-compatible float -10.500000 in %s on line %d
string(22) "1777777777777777777766"

-- Iteration 9 --
string(13) "1627646215010"

-- Iteration 10 --

Deprecated: Implicit conversion to int from non-compatible float 0.000000 in %s on line %d
string(1) "0"

-- Iteration 11 --

Deprecated: Implicit conversion to int from non-compatible float 0.500000 in %s on line %d
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
decoct(): Argument #1 ($num) must be of type int, string given

-- Iteration 17 --
decoct(): Argument #1 ($num) must be of type int, string given

-- Iteration 18 --
decoct(): Argument #1 ($num) must be of type int, array given

-- Iteration 19 --
decoct(): Argument #1 ($num) must be of type int, string given

-- Iteration 20 --
decoct(): Argument #1 ($num) must be of type int, string given

-- Iteration 21 --
decoct(): Argument #1 ($num) must be of type int, string given

-- Iteration 22 --
decoct(): Argument #1 ($num) must be of type int, classA given

-- Iteration 23 --
decoct(): Argument #1 ($num) must be of type int, resource given
