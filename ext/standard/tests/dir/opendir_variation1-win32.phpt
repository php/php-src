--TEST--
Test opendir() function : usage variations - different data types as $path arg
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/* Prototype  : mixed opendir(string $path[, resource $context])
 * Description: Open a directory and return a dir_handle 
 * Source code: ext/standard/dir.c
 */

/*
 * Pass different data types as $path argument to opendir() to test behaviour
 * Where possible, an existing directory has been entered as a string value
 */

echo "*** Testing opendir() : usage variations ***\n";

// create directory to be passed as string value where possible
$path = dirname(__FILE__) . "/opendir_variation1";
mkdir($path);

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA {
	
	var $path;
	function __construct($path) {
		$this->path = $path;
	}
	public function __toString() {
		return $this->path;
	}
}

// heredoc string
$heredoc = <<<EOT
$path
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $path argument
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
       array(),

       // string data
/*19*/ "$path",
       'string',
       $heredoc,
       
       // object data
/*22*/ new classA($path),

       // undefined data
/*23*/ @$undefined_var,

       // unset data
/*24*/ @$unset_var,

       // resource variable
/*25*/ $fp
);

// loop through each element of $inputs to check the behavior of opendir()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump( $dh = opendir($input) );
	if ($dh) {
		closedir($dh);
	}
	$iterator++;
};

fclose($fp);
?>
===DONE===
--CLEAN--
<?php
$path = dirname(__FILE__) . "/opendir_variation1";
rmdir($path);
?>
--EXPECTF--
*** Testing opendir() : usage variations ***

-- Iteration 1 --

Warning: opendir(0,0): The system cannot find the file specified. (code: 2) in %s on line %d

Warning: opendir(0): failed to open dir: %s in %s on line %d
bool(false)

-- Iteration 2 --

Warning: opendir(1,1): The system cannot find the file specified. (code: 2) in %s on line %d

Warning: opendir(1): failed to open dir: %s in %s on line %d
bool(false)

-- Iteration 3 --

Warning: opendir(12345,12345): The system cannot find the file specified. (code: 2) in %s on line %d

Warning: opendir(12345): failed to open dir: %s in %s on line %d
bool(false)

-- Iteration 4 --

Warning: opendir(-2345,-2345): The system cannot find the file specified. (code: 2) in %s on line %d

Warning: opendir(-2345): failed to open dir: %s in %s on line %d
bool(false)

-- Iteration 5 --

Warning: opendir(10.5,10.5): The system cannot find the file specified. (code: 2) in %s on line %d

Warning: opendir(10.5): failed to open dir: %s in %s on line %d
bool(false)

-- Iteration 6 --

Warning: opendir(-10.5,-10.5): The system cannot find the file specified. (code: 2) in %s on line %d

Warning: opendir(-10.5): failed to open dir: %s in %s on line %d
bool(false)

-- Iteration 7 --

Warning: opendir(123456789000,123456789000): The system cannot find the file specified. (code: 2) in %s on line %d

Warning: opendir(123456789000): failed to open dir: %s in %s on line %d
bool(false)

-- Iteration 8 --

Warning: opendir(1.23456789E-9,1.23456789E-9): The system cannot find the file specified. (code: 2) in %s on line %d

Warning: opendir(1.23456789E-9): failed to open dir: %s in %s on line %d
bool(false)

-- Iteration 9 --

Warning: opendir(0.5,0.5): The system cannot find the file specified. (code: 2) in %s on line %d

Warning: opendir(0.5): failed to open dir: %s in %s on line %d
bool(false)

-- Iteration 10 --
bool(false)

-- Iteration 11 --
bool(false)

-- Iteration 12 --

Warning: opendir(1,1): The system cannot find the file specified. (code: 2) in %s on line %d

Warning: opendir(1): failed to open dir: %s in %s on line %d
bool(false)

-- Iteration 13 --
bool(false)

-- Iteration 14 --

Warning: opendir(1,1): The system cannot find the file specified. (code: 2) in %s on line %d

Warning: opendir(1): failed to open dir: %s in %s on line %d
bool(false)

-- Iteration 15 --
bool(false)

-- Iteration 16 --
bool(false)

-- Iteration 17 --
bool(false)

-- Iteration 18 --

Warning: opendir() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration 19 --
resource(%d) of type (stream)

-- Iteration 20 --

Warning: opendir(string,string): The system cannot find the file specified. (code: 2) in %s on line %d

Warning: opendir(string): failed to open dir: %s in %s on line %d
bool(false)

-- Iteration 21 --
resource(%d) of type (stream)

-- Iteration 22 --
resource(%d) of type (stream)

-- Iteration 23 --
bool(false)

-- Iteration 24 --
bool(false)

-- Iteration 25 --

Warning: opendir() expects parameter 1 to be string, resource given in %s on line %d
NULL
===DONE===
