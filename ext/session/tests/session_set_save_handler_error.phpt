--TEST--
Test session_set_save_handler() function : error functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : error functionality ***\n";

// Get an unset variable
$unset_var = 10;
unset($unset_var);

class classA
{
    public function __toString() {
        return "Hello World!";
    }
}

$heredoc = <<<EOT
Hello World!
EOT;

$fp = fopen(__FILE__, "r");

// Unexpected values to be passed as arguments
$inputs = array(

       // Integer data
/*1*/  0,
       1,
       12345,
       -2345,

       // Float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // Null data
/*10*/ NULL,
       null,

       // Boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,

       // Empty strings
/*16*/ "",
       '',

       // Invalid string data
/*18*/ "Nothing",
       'Nothing',
       $heredoc,

       // Object data
/*21*/ new classA(),

       // Undefined data
/*22*/ @$undefined_var,

       // Unset data
/*23*/ @$unset_var,

       // Resource variable
/*24*/ $fp
);


$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    try {
        session_set_save_handler($input, NULL, NULL, NULL, NULL, NULL);
    } catch (TypeError $exception) {
        echo $exception->getMessage() . "\n";
    }
    $iterator++;
}

fclose($fp);
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_save_handler() : error functionality ***

-- Iteration 1 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "0" not found or invalid function name

-- Iteration 2 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "1" not found or invalid function name

-- Iteration 3 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "12345" not found or invalid function name

-- Iteration 4 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "-2345" not found or invalid function name

-- Iteration 5 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "10.5" not found or invalid function name

-- Iteration 6 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "-10.5" not found or invalid function name

-- Iteration 7 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "123456789000" not found or invalid function name

-- Iteration 8 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "1.23456789E-9" not found or invalid function name

-- Iteration 9 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "0.5" not found or invalid function name

-- Iteration 10 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "" not found or invalid function name

-- Iteration 11 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "" not found or invalid function name

-- Iteration 12 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "1" not found or invalid function name

-- Iteration 13 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "" not found or invalid function name

-- Iteration 14 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "1" not found or invalid function name

-- Iteration 15 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "" not found or invalid function name

-- Iteration 16 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "" not found or invalid function name

-- Iteration 17 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "" not found or invalid function name

-- Iteration 18 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "Nothing" not found or invalid function name

-- Iteration 19 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "Nothing" not found or invalid function name

-- Iteration 20 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "Hello World!" not found or invalid function name

-- Iteration 21 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "classA::__invoke" not found or invalid function name

-- Iteration 22 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "" not found or invalid function name

-- Iteration 23 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "" not found or invalid function name

-- Iteration 24 --
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "Resource id #%d" not found or invalid function name
Done
