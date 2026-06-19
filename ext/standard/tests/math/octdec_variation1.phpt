--TEST--
Test octdec() function : usage variations - different data types as $octal_string arg
--INI--
precision=14
--FILE--
<?php
echo "*** Testing octdec() : usage variations ***\n";

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

       // resource variable
/*26*/ $fp
);

// loop through each element of $inputs to check the behaviour of octdec()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump(octdec($input));
    } catch (TypeError $e) {
        echo 'TypeError: ', $e->getMessage(), "\n";
    } catch (ValueError $e) {
        echo 'ValueError: ', $e->getMessage(), "\n";
    }
    $iterator++;
};
fclose($fp);
?>
---Done---
--EXPECT--
*** Testing octdec() : usage variations ***

-- Iteration 1 --
int(0)

-- Iteration 2 --
int(1)

-- Iteration 3 --
int(5349)

-- Iteration 4 --
ValueError: Invalid characters passed for attempted conversion

-- Iteration 5 --
ValueError: Invalid characters passed for attempted conversion

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
ValueError: Invalid characters passed for attempted conversion

-- Iteration 12 --
int(1)

-- Iteration 13 --
int(0)

-- Iteration 14 --
int(1)

-- Iteration 15 --
int(0)

-- Iteration 16 --
int(0)

-- Iteration 17 --
int(0)

-- Iteration 18 --
TypeError: octdec(): Argument #1 ($octal_string) must be of type string, array given

-- Iteration 19 --
ValueError: Invalid characters passed for attempted conversion

-- Iteration 20 --
ValueError: Invalid characters passed for attempted conversion

-- Iteration 21 --
ValueError: Invalid characters passed for attempted conversion

-- Iteration 22 --
TypeError: octdec(): Argument #1 ($octal_string) must be of type string, resource given
---Done---
