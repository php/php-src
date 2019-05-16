--TEST--
Test session_cache_limiter() function : error functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : string session_cache_limiter([string $cache_limiter])
 * Description : Get and/or set the current cache limiter
 * Source code : ext/session/session.c
 */

echo "*** Testing session_cache_limiter() : error functionality ***\n";

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
);


$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    var_dump(session_cache_limiter($input));
    $iterator++;
};

fclose($fp);
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_cache_limiter() : error functionality ***

-- Iteration 1 --
string(7) "nocache"

-- Iteration 2 --
string(1) "0"

-- Iteration 3 --
string(1) "1"

-- Iteration 4 --
string(5) "12345"

-- Iteration 5 --
string(5) "-2345"

-- Iteration 6 --
string(4) "10.5"

-- Iteration 7 --
string(5) "-10.5"

-- Iteration 8 --
string(12) "123456789000"

-- Iteration 9 --
string(13) "1.23456789E-9"

-- Iteration 10 --
string(3) "0.5"

-- Iteration 11 --
string(0) ""

-- Iteration 12 --
string(0) ""

-- Iteration 13 --
string(1) "1"

-- Iteration 14 --
string(0) ""

-- Iteration 15 --
string(1) "1"

-- Iteration 16 --
string(0) ""

-- Iteration 17 --
string(0) ""

-- Iteration 18 --
string(0) ""

-- Iteration 19 --
string(7) "Nothing"

-- Iteration 20 --
string(7) "Nothing"

-- Iteration 21 --
string(12) "Hello World!"

-- Iteration 22 --
string(12) "Hello World!"

-- Iteration 23 --
string(0) ""
Done
