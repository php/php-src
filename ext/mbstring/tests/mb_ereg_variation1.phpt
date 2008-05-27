--TEST--
Test mb_ereg() function : usage variations - pass different data types to $pattern argument
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_ereg') or die("skip mb_ereg() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_ereg(string $pattern, string $string [, array $registers])
 * Description: Regular expression match for multibyte string 
 * Source code: ext/mbstring/php_mbregex.c 
 */

/*
 * Pass different data types to $pattern argument
 */

echo "*** Testing mb_ereg() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$string = 'string value';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
	public function __toString() {
		return "Class A object";
	}
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $pattern argument
$inputs = array(

// int data
/*1*/  0,
       1,
       12345,
       -2345,

// float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

// null data
/*10*/ NULL,
       null,

// boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,
 
// empty data
/*16*/ "",
       '',

// string data
/*18*/ "string",
       'string',
       $heredoc,
 
// object data
/*21*/ new classA(),

// undefined data
/*22*/ @$undefined_var,

// unset data
/*23*/ @$unset_var,

// resource variable
/*24*/ $fp
);

// loop through each element of $inputs to check the behavior of mb_ereg()
$iterator = 1;
foreach($inputs as $input) {
	if (@is_array($regs)){
		$regs = null;
	}
	echo "\n-- Iteration $iterator --\n";
	var_dump( mb_ereg($input, $string, $regs) );
	var_dump($regs);
	$iterator++;
};

fclose($fp);

echo "Done";
?>

--EXPECT--
*** Testing mb_ereg() : usage variations ***

-- Iteration 1 --
bool(false)
NULL

-- Iteration 2 --
bool(false)
NULL

-- Iteration 3 --
bool(false)
NULL

-- Iteration 4 --
bool(false)
NULL

-- Iteration 5 --
bool(false)
NULL

-- Iteration 6 --
bool(false)
NULL

-- Iteration 7 --
bool(false)
NULL

-- Iteration 8 --
bool(false)
NULL

-- Iteration 9 --
bool(false)
NULL

-- Iteration 10 --
int(1)
bool(false)
NULL

-- Iteration 11 --
int(1)
bool(false)
NULL

-- Iteration 12 --
bool(false)
NULL

-- Iteration 13 --
int(1)
bool(false)
NULL

-- Iteration 14 --
bool(false)
NULL

-- Iteration 15 --
int(1)
bool(false)
NULL

-- Iteration 16 --
int(1)
bool(false)
NULL

-- Iteration 17 --
int(1)
bool(false)
NULL

-- Iteration 18 --
int(6)
array(1) {
  [0]=>
  string(6) "string"
}

-- Iteration 19 --
int(6)
array(1) {
  [0]=>
  string(6) "string"
}

-- Iteration 20 --
bool(false)
NULL

-- Iteration 21 --
bool(false)
NULL

-- Iteration 22 --
int(1)
bool(false)
NULL

-- Iteration 23 --
int(1)
bool(false)
NULL

-- Iteration 24 --
bool(false)
NULL
Done
