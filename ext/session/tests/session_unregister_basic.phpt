--TEST--
Test session_unregister() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_unregister(string $name)
 * Description : Unregister a global variable from the current session
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_unregister() : basic functionality ***\n";

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


var_dump(session_start());

$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    var_dump(session_unregister($input));
    $iterator++;
};

var_dump(session_destroy());

fclose($fp);
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_unregister() : basic functionality ***
bool(true)

-- Iteration 1 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 2 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 3 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 4 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 5 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 6 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 7 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 8 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 9 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 10 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 11 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 12 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 13 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 14 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 15 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 16 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 17 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 18 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 19 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 20 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 21 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 22 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 23 --

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)

-- Iteration 24 --

Deprecated: Function session_unregister() is deprecated in %s on line %d

Warning: session_unregister() expects parameter 1 to be string, resource given in %s on line %d
NULL
bool(true)
Done
