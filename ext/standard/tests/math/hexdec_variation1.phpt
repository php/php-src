--TEST--
Test hexdec() function : usage variations - different data types as $number arg
--INI--
precision=14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
echo "*** Testing hexdec() : usage variations ***\n";

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

// loop through each element of $inputs to check the behaviour of hexdec()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump(hexdec($input));
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
*** Testing hexdec() : usage variations ***

-- Iteration 1 --
int(0)

-- Iteration 2 --
int(1)

-- Iteration 3 --
int(74565)

-- Iteration 4 --
ValueError: hexdec(): Argument #1 ($hex_string) has invalid characters for attempted conversion

-- Iteration 5 --
float(285960729237)

-- Iteration 6 --
float(285960729238)

-- Iteration 7 --
ValueError: hexdec(): Argument #1 ($hex_string) has invalid characters for attempted conversion

-- Iteration 8 --
ValueError: hexdec(): Argument #1 ($hex_string) has invalid characters for attempted conversion

-- Iteration 9 --
float(20015998341120)

-- Iteration 10 --
ValueError: hexdec(): Argument #1 ($hex_string) has invalid characters for attempted conversion

-- Iteration 11 --
ValueError: hexdec(): Argument #1 ($hex_string) has invalid characters for attempted conversion

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
TypeError: hexdec(): Argument #1 ($hex_string) must be of type string, array given

-- Iteration 19 --
ValueError: hexdec(): Argument #1 ($hex_string) has invalid characters for attempted conversion

-- Iteration 20 --
ValueError: hexdec(): Argument #1 ($hex_string) has invalid characters for attempted conversion

-- Iteration 21 --
ValueError: hexdec(): Argument #1 ($hex_string) has invalid characters for attempted conversion

-- Iteration 22 --
TypeError: hexdec(): Argument #1 ($hex_string) must be of type string, resource given
