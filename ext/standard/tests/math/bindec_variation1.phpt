--TEST--
Test bindec() function : usage variations - different data types as $binary_string arg
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
echo "*** Testing bindec() : usage variations ***\n";

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

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*16*/ "",
       '',
       array(),

       // string data
/*19*/ "abcxyz",
       'abcxyz',
       $heredoc,

       // resource variable
/*24*/ $fp
);

// loop through each element of $inputs to check the behaviour of bindec()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump(bindec($input));
    } catch (TypeError $e) {
        echo 'TypeError: ', $e->getMessage(), "\n";
    } catch (ValueError $e) {
        echo 'ValueError: ', $e->getMessage(), "\n";
    }
    $iterator++;
};
fclose($fp);
?>
--EXPECT--
*** Testing bindec() : usage variations ***

-- Iteration 1 --
int(0)

-- Iteration 2 --
int(1)

-- Iteration 3 --
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion

-- Iteration 4 --
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion

-- Iteration 5 --
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion

-- Iteration 6 --
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion

-- Iteration 7 --
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion

-- Iteration 8 --
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion

-- Iteration 9 --
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion

-- Iteration 10 --
int(1)

-- Iteration 11 --
int(0)

-- Iteration 12 --
int(1)

-- Iteration 13 --
int(0)

-- Iteration 14 --
int(0)

-- Iteration 15 --
int(0)

-- Iteration 16 --
TypeError: bindec(): Argument #1 ($binary_string) must be of type string, array given

-- Iteration 17 --
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion

-- Iteration 18 --
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion

-- Iteration 19 --
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion

-- Iteration 20 --
TypeError: bindec(): Argument #1 ($binary_string) must be of type string, resource given
