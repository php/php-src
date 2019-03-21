--TEST--
Test scandir() function : usage variations - different file names
--FILE--
<?php
/* Prototype  : array scandir(string $dir [, int $sorting_order [, resource $context]])
 * Description: List files & directories inside the specified path
 * Source code: ext/standard/dir.c
 */

/*
 * Pass a directory containing files with different types of names to test how scandir()
 * reads them
 */

echo "*** Testing scandir() : usage variations ***\n";

$dir_path = __DIR__ . "/scandir_variation8/";
mkdir($dir_path);

// heredoc string
$heredoc = <<<EOT
hd_file
EOT;

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

       // empty data
/*10*/ "",
       array(),

       // string data
/*12*/ "double_file",
       'single_file',
       $heredoc,
);

$iterator = 1;
foreach($inputs as $key => $input) {
	echo "\n-- Iteration $iterator --\n";
	$handle = "fp{$iterator}";
	var_dump( $$handle = fopen(@"$dir_path$input.tmp", 'w') );
	fclose($$handle);
	$iterator++;
};

echo "\n-- Call to scandir() --\n";
var_dump($content = scandir($dir_path));

// remove all files in directory so can remove directory in CLEAN section
foreach ($content as $file_name) {
	// suppress errors as won't be able to remove "." and ".." entries
	@unlink($dir_path . $file_name);
}
?>
===DONE===
--CLEAN--
<?php
$dir_path = __DIR__ . "/scandir_variation8";
rmdir($dir_path);
?>
--EXPECTF--
*** Testing scandir() : usage variations ***

-- Iteration 1 --
resource(%d) of type (stream)

-- Iteration 2 --
resource(%d) of type (stream)

-- Iteration 3 --
resource(%d) of type (stream)

-- Iteration 4 --
resource(%d) of type (stream)

-- Iteration 5 --
resource(%d) of type (stream)

-- Iteration 6 --
resource(%d) of type (stream)

-- Iteration 7 --
resource(%d) of type (stream)

-- Iteration 8 --
resource(%d) of type (stream)

-- Iteration 9 --
resource(%d) of type (stream)

-- Iteration 10 --
resource(%d) of type (stream)

-- Iteration 11 --
resource(%d) of type (stream)

-- Iteration 12 --
resource(%d) of type (stream)

-- Iteration 13 --
resource(%d) of type (stream)

-- Iteration 14 --
resource(%d) of type (stream)

-- Call to scandir() --
array(16) {
  [0]=>
  string(9) "-10.5.tmp"
  [1]=>
  string(9) "-2345.tmp"
  [2]=>
  string(1) "."
  [3]=>
  string(2) ".."
  [4]=>
  string(4) ".tmp"
  [5]=>
  string(7) "0.5.tmp"
  [6]=>
  string(5) "0.tmp"
  [7]=>
  string(17) "1.23456789E-9.tmp"
  [8]=>
  string(5) "1.tmp"
  [9]=>
  string(8) "10.5.tmp"
  [10]=>
  string(9) "12345.tmp"
  [11]=>
  string(16) "123456789000.tmp"
  [12]=>
  string(9) "Array.tmp"
  [13]=>
  string(15) "double_file.tmp"
  [14]=>
  string(11) "hd_file.tmp"
  [15]=>
  string(15) "single_file.tmp"
}
===DONE===
