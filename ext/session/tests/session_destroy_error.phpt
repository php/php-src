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
	session_start();
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
bool(true)

-- Iteration 2 --

Notice: session_destroy(): Immediate session data removal may cause random lost sessions. It is advised not to delete immediately in %s on line %d
bool(true)

-- Iteration 3 --

Notice: session_destroy(): Immediate session data removal may cause random lost sessions. It is advised not to delete immediately in %s on line %d
bool(true)

-- Iteration 4 --

Notice: session_destroy(): Immediate session data removal may cause random lost sessions. It is advised not to delete immediately in %s on line %d
bool(true)

-- Iteration 5 --

Notice: session_destroy(): Immediate session data removal may cause random lost sessions. It is advised not to delete immediately in %s on line %d
bool(true)

-- Iteration 6 --

Notice: session_destroy(): Immediate session data removal may cause random lost sessions. It is advised not to delete immediately in %s on line %d
bool(true)

-- Iteration 7 --

Notice: session_destroy(): Immediate session data removal may cause random lost sessions. It is advised not to delete immediately in %s on line %d
bool(true)

-- Iteration 8 --

Notice: session_destroy(): Immediate session data removal may cause random lost sessions. It is advised not to delete immediately in %s on line %d
bool(true)

-- Iteration 9 --

Notice: session_destroy(): Immediate session data removal may cause random lost sessions. It is advised not to delete immediately in %s on line %d
bool(true)

-- Iteration 10 --
bool(true)

-- Iteration 11 --
bool(true)

-- Iteration 12 --

Notice: session_destroy(): Immediate session data removal may cause random lost sessions. It is advised not to delete immediately in %s on line %d
bool(true)

-- Iteration 13 --
bool(true)

-- Iteration 14 --

Notice: session_destroy(): Immediate session data removal may cause random lost sessions. It is advised not to delete immediately in %s on line %d
bool(true)

-- Iteration 15 --
bool(true)

-- Iteration 16 --
bool(true)

-- Iteration 17 --
bool(true)

-- Iteration 18 --

Notice: session_destroy(): Immediate session data removal may cause random lost sessions. It is advised not to delete immediately in %s on line %d
bool(true)

-- Iteration 19 --

Notice: session_destroy(): Immediate session data removal may cause random lost sessions. It is advised not to delete immediately in %s on line %d
bool(true)

-- Iteration 20 --

Notice: session_destroy(): Immediate session data removal may cause random lost sessions. It is advised not to delete immediately in %s on line %d
bool(true)

-- Iteration 21 --

Warning: session_destroy() expects parameter 1 to be boolean, object given in %s on line %d
NULL

-- Iteration 22 --

Notice: A session had already been started - ignoring session_start() in %s on line 82
bool(true)

-- Iteration 23 --
bool(true)

-- Iteration 24 --

Warning: session_destroy() expects parameter 1 to be boolean, resource given in %s on line %d
NULL
Done
