--TEST--
Test session_name() function : error functionality
--INI--
session.save_path=
session.name=PHPSESSID
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : string session_name([string $name])
 * Description : Get and/or set the current session name
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_name() : error functionality ***\n";

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
    var_dump($input, session_name($input));
    $iterator++;
};

fclose($fp);
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_name() : error functionality ***

-- Iteration 1 --

Warning: session_name(): session.name cannot be a numeric or empty '0' in %s on line %d
int(0)
string(9) "PHPSESSID"

-- Iteration 2 --

Warning: session_name(): session.name cannot be a numeric or empty '1' in %s on line %d
int(1)
string(9) "PHPSESSID"

-- Iteration 3 --

Warning: session_name(): session.name cannot be a numeric or empty '12345' in %s on line %d
int(12345)
string(9) "PHPSESSID"

-- Iteration 4 --

Warning: session_name(): session.name cannot be a numeric or empty '-2345' in %s on line %d
int(-2345)
string(9) "PHPSESSID"

-- Iteration 5 --

Warning: session_name(): session.name cannot be a numeric or empty '10.5' in %s on line %d
float(10.5)
string(9) "PHPSESSID"

-- Iteration 6 --

Warning: session_name(): session.name cannot be a numeric or empty '-10.5' in %s on line %d
float(-10.5)
string(9) "PHPSESSID"

-- Iteration 7 --

Warning: session_name(): session.name cannot be a numeric or empty '123456789000' in %s on line %d
float(123456789000)
string(9) "PHPSESSID"

-- Iteration 8 --

Warning: session_name(): session.name cannot be a numeric or empty '1.23456789E-9' in %s on line %d
float(1.23456789E-9)
string(9) "PHPSESSID"

-- Iteration 9 --

Warning: session_name(): session.name cannot be a numeric or empty '0.5' in %s on line %d
float(0.5)
string(9) "PHPSESSID"

-- Iteration 10 --

Warning: session_name(): session.name cannot be a numeric or empty '' in %s on line %d
NULL
string(9) "PHPSESSID"

-- Iteration 11 --

Warning: session_name(): session.name cannot be a numeric or empty '' in %s on line %d
NULL
string(9) "PHPSESSID"

-- Iteration 12 --

Warning: session_name(): session.name cannot be a numeric or empty '1' in %s on line %d
bool(true)
string(9) "PHPSESSID"

-- Iteration 13 --

Warning: session_name(): session.name cannot be a numeric or empty '' in %s on line %d
bool(false)
string(9) "PHPSESSID"

-- Iteration 14 --

Warning: session_name(): session.name cannot be a numeric or empty '1' in %s on line %d
bool(true)
string(9) "PHPSESSID"

-- Iteration 15 --

Warning: session_name(): session.name cannot be a numeric or empty '' in %s on line %d
bool(false)
string(9) "PHPSESSID"

-- Iteration 16 --

Warning: session_name(): session.name cannot be a numeric or empty '' in %s on line %d
string(0) ""
string(9) "PHPSESSID"

-- Iteration 17 --

Warning: session_name(): session.name cannot be a numeric or empty '' in %s on line %d
string(0) ""
string(9) "PHPSESSID"

-- Iteration 18 --
string(7) "Nothing"
string(9) "PHPSESSID"

-- Iteration 19 --
string(7) "Nothing"
string(7) "Nothing"

-- Iteration 20 --
string(12) "Hello World!"
string(7) "Nothing"

-- Iteration 21 --
object(classA)#1 (0) {
}
string(12) "Hello World!"

-- Iteration 22 --

Warning: session_name(): session.name cannot be a numeric or empty '' in %s on line %d
NULL
string(12) "Hello World!"

-- Iteration 23 --

Warning: session_name(): session.name cannot be a numeric or empty '' in %s on line %d
NULL
string(12) "Hello World!"

-- Iteration 24 --

Warning: session_name() expects parameter 1 to be string, resource given in %s on line %d
resource(%d) of type (stream)
NULL
Done