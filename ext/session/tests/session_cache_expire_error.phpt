--TEST--
Test session_cache_expire() function : error functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : int session_cache_expire([int $new_cache_expire])
 * Description : Return current cache expire
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_cache_expire() : error functionality ***\n";

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
    var_dump(session_cache_expire($input));
    $iterator++;
};

fclose($fp);
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_cache_expire() : error functionality ***

-- Iteration 1 --
int(180)

-- Iteration 2 --
int(0)

-- Iteration 3 --
int(1)

-- Iteration 4 --
int(12345)

-- Iteration 5 --
int(-2345)

-- Iteration 6 --
int(10)

-- Iteration 7 --
int(-10)

-- Iteration 8 --
int(%s)

-- Iteration 9 --
int(1)

-- Iteration 10 --
int(0)

-- Iteration 11 --
int(0)

-- Iteration 12 --
int(0)

-- Iteration 13 --
int(1)

-- Iteration 14 --
int(0)

-- Iteration 15 --
int(1)

-- Iteration 16 --
int(0)

-- Iteration 17 --
int(0)

-- Iteration 18 --
int(0)

-- Iteration 19 --
int(0)

-- Iteration 20 --
int(0)

-- Iteration 21 --
int(0)

-- Iteration 22 --
int(0)

-- Iteration 23 --
int(0)

-- Iteration 24 --
int(0)
Done

