--TEST--
Test session_destroy() function : error functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_destroy(void)
 * Description : Destroys all data registered to a session
 * Source code : ext/session/session.c
 */

echo "*** Testing session_destroy() : error functionality ***\n";

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
    var_dump(session_destroy($input));
    $iterator++;
};

fclose($fp);
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_destroy() : error functionality ***

-- Iteration 1 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 2 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 3 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 4 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 5 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 6 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 7 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 8 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 9 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 10 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 11 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 12 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 13 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 14 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 15 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 16 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 17 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 18 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 19 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 20 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 21 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 22 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 23 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL

-- Iteration 24 --

Warning: session_destroy() expects exactly 0 parameters, 1 given in %s on line %d
NULL
Done
