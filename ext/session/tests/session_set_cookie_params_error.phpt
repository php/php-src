--TEST--
Test session_set_cookie_params() function : error functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : void session_set_cookie_params(int $lifetime [, string $path [, string $domain [, bool $secure [, bool $httponly]]]])
 * Description : Set the session cookie parameters
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_set_cookie_params() : error functionality ***\n";

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
    var_dump(session_set_cookie_params($input));
    var_dump(session_set_cookie_params(1234567890, $input));
    var_dump(session_set_cookie_params(1234567890, "blah", $input));
    var_dump(session_set_cookie_params(1234567890, "blah", "foo", $input));
    var_dump(session_set_cookie_params(1234567890, "blah", "foo", TRUE, $input));
    var_dump(session_set_cookie_params(1234567890, "blah", "foo", TRUE, FALSE));
    $iterator++;
};

fclose($fp);
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_cookie_params() : error functionality ***

-- Iteration 1 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 2 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 3 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 4 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 5 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 6 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 7 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 8 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 9 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 10 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 11 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 12 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 13 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 14 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 15 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 16 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 17 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 18 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 19 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 20 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 21 --
NULL
NULL
NULL

Warning: session_set_cookie_params() expects parameter 4 to be boolean, object given in %s on line %d
NULL

Warning: session_set_cookie_params() expects parameter 5 to be boolean, object given in %s on line %d
NULL
NULL

-- Iteration 22 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 23 --
NULL
NULL
NULL
NULL
NULL
NULL

-- Iteration 24 --
NULL

Warning: session_set_cookie_params() expects parameter 2 to be string, resource given in %s on line %d
NULL

Warning: session_set_cookie_params() expects parameter 3 to be string, resource given in %s on line %d
NULL

Warning: session_set_cookie_params() expects parameter 4 to be boolean, resource given in %s on line %d
NULL

Warning: session_set_cookie_params() expects parameter 5 to be boolean, resource given in %s on line %d
NULL
NULL
Done

