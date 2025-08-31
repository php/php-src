--TEST--
Test session_encode() function : error functionality
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_encode() : error functionality ***\n";

// Unexpected values to be passed as arguments
$inputs = array(

       // Integer data
/*1*/  0,
       1,
       12345,
       -2345,

       // Empty strings
       '',

       // Invalid string data
       'Nothing',
);

$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    var_dump(session_start());
    try {
        $_SESSION[$input] = "Hello World!";
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump(session_encode());
    var_dump(session_destroy());
    $iterator++;
};

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_encode() : error functionality ***

-- Iteration 1 --
bool(true)

Warning: session_encode(): Skipping numeric key 0 in %s on line %d
bool(false)
bool(true)

-- Iteration 2 --
bool(true)

Warning: session_encode(): Skipping numeric key 1 in %s on line %d
bool(false)
bool(true)

-- Iteration 3 --
bool(true)

Warning: session_encode(): Skipping numeric key 12345 in %s on line %d
bool(false)
bool(true)

-- Iteration 4 --
bool(true)

Warning: session_encode(): Skipping numeric key -2345 in %s on line %d
bool(false)
bool(true)

-- Iteration 5 --
bool(true)
string(21) "|s:12:"Hello World!";"
bool(true)

-- Iteration 6 --
bool(true)
string(28) "Nothing|s:12:"Hello World!";"
bool(true)
Done
