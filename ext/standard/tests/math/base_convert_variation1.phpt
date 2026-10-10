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
       // string data
/*20*/ "abcxyz",
       'abcxyz',
       $heredoc,
);

// loop through each element of $inputs to check the behaviour of base_convert()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    var_dump(base_convert($input, 10, 8));
    $iterator++;
}
?>
--EXPECTF--
*** Testing base_convert() : usage variations ***

-- Iteration 1 --
string(1) "0"

-- Iteration 2 --
string(1) "1"

-- Iteration 3 --
string(2) "14"

-- Iteration 4 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
string(2) "14"

-- Iteration 5 --
string(11) "17777777777"

-- Iteration 6 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
string(3) "151"

-- Iteration 7 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
string(3) "151"

-- Iteration 8 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
string(7) "4553207"

-- Iteration 9 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
string(7) "4553207"

-- Iteration 10 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
string(1) "5"

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

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
string(1) "0"

-- Iteration 18 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
string(1) "0"

-- Iteration 19 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
string(1) "0"
