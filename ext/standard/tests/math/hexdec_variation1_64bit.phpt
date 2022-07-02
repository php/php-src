--TEST--
Test hexdec() function : usage variations - different data types as $number arg
--INI--
precision=14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
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
        echo $e->getMessage(), "\n";
    }
    $iterator++;
};
fclose($fp);
?>
--EXPECTF--
*** Testing hexdec() : usage variations ***

-- Iteration 1 --
int(0)

-- Iteration 2 --
int(1)

-- Iteration 3 --
int(74565)

-- Iteration 4 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(9029)

-- Iteration 5 --
int(285960729237)

-- Iteration 6 --
int(285960729238)

-- Iteration 7 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(261)

-- Iteration 8 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(261)

-- Iteration 9 --
int(20015998341120)

-- Iteration 10 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(1250999896553)

-- Iteration 11 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(5)

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
hexdec(): Argument #1 ($hex_string) must be of type string, array given

-- Iteration 19 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(2748)

-- Iteration 20 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(2748)

-- Iteration 21 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(2748)

-- Iteration 22 --
hexdec(): Argument #1 ($hex_string) must be of type string, resource given
