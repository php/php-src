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
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 2 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 3 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 4 --

Warning: session_set_cookie_params(): CookieLifetime cannot be negative in %s on line 81
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 5 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 6 --

Warning: session_set_cookie_params(): CookieLifetime cannot be negative in %s on line 81
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 7 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 8 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 9 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 10 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 11 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 12 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 13 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 14 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 15 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 16 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 17 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 18 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 19 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 20 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 21 --
bool(true)
bool(true)
bool(true)

Warning: session_set_cookie_params() expects parameter 4 to be bool, object given in %s on line 84
NULL

Warning: session_set_cookie_params() expects parameter 5 to be bool, object given in %s on line 85
NULL
bool(true)

-- Iteration 22 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 23 --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- Iteration 24 --
bool(true)

Warning: session_set_cookie_params() expects parameter 2 to be string, resource given in %s on line 82
NULL

Warning: session_set_cookie_params() expects parameter 3 to be string, resource given in %s on line 83
NULL

Warning: session_set_cookie_params() expects parameter 4 to be bool, resource given in %s on line 84
NULL

Warning: session_set_cookie_params() expects parameter 5 to be bool, resource given in %s on line 85
NULL
bool(true)
Done
