--TEST--
Test base_convert() function : usage variations - different data types as $number argument
--FILE--
<?php
echo "*** Testing base_convert() : usage variations ***\n";

// heredoc string
$heredoc = <<<EOT
abc
xyz
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

$inputs = array(
       // int data
/*1*/  0,
       1,
       12,
       -12,
       2147483647,

       // float data
/*6*/  10.5,
       -10.5,
       1.234567e2,
       1.234567E-2,
       .5,

       // boolean data
/*13*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*17*/ "",
       '',
       array(),

       // string data
/*20*/ "abcxyz",
       'abcxyz',
       $heredoc,

       // resource variable
/*25*/ $fp
);

// loop through each element of $inputs to check the behaviour of base_convert()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump(base_convert($input, 10, 8));
    } catch (TypeError $exception) {
        echo 'TypeError: ', $exception->getMessage() . "\n";
    } catch (ValueError $exception) {
        echo 'ValueError: ', $exception->getMessage() . "\n";
    }
    $iterator++;
}
fclose($fp);
?>
--EXPECT--
*** Testing base_convert() : usage variations ***

-- Iteration 1 --
string(1) "0"

-- Iteration 2 --
string(1) "1"

-- Iteration 3 --
string(2) "14"

-- Iteration 4 --
ValueError: Invalid characters passed for attempted conversion

-- Iteration 5 --
string(11) "17777777777"

-- Iteration 6 --
ValueError: Invalid characters passed for attempted conversion

-- Iteration 7 --
ValueError: Invalid characters passed for attempted conversion

-- Iteration 8 --
ValueError: Invalid characters passed for attempted conversion

-- Iteration 9 --
ValueError: Invalid characters passed for attempted conversion

-- Iteration 10 --
ValueError: Invalid characters passed for attempted conversion

-- Iteration 11 --
string(1) "1"

-- Iteration 12 --
string(1) "0"

-- Iteration 13 --
string(1) "1"

-- Iteration 14 --
string(1) "0"

-- Iteration 15 --
string(1) "0"

-- Iteration 16 --
string(1) "0"

-- Iteration 17 --
TypeError: base_convert(): Argument #1 ($num) must be of type string, array given

-- Iteration 18 --
ValueError: Invalid characters passed for attempted conversion

-- Iteration 19 --
ValueError: Invalid characters passed for attempted conversion

-- Iteration 20 --
ValueError: Invalid characters passed for attempted conversion

-- Iteration 21 --
TypeError: base_convert(): Argument #1 ($num) must be of type string, resource given
