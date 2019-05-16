--TEST--
Test readdir() function : usage variations - different file names
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/* Prototype  : string readdir([resource $dir_handle])
 * Description: Read directory entry from dir_handle
 * Source code: ext/standard/dir.c
 */

/*
 * Pass a directory handle pointing to a directory that contains
 * files with different file names to test how readdir() reads them
 */

echo "*** Testing readdir() : usage variations ***\n";

$dir_path = __DIR__ . "/私はガラスを食べられますreaddir_variation4/";
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
	var_dump( $$handle = @fopen($dir_path . "私はガラスを食べられます$input.tmp", 'w') );
	var_dump( fwrite($$handle, $key));
	fclose($$handle);
	$iterator++;
};

echo "\n-- Call to readdir() --\n";
$dir_handle = opendir($dir_path);
while(FALSE !== ($file = readdir($dir_handle))){

	// different OS order files differently so will
	// store file names into an array so can use sorted in expected output
	$contents[] = $file;

	// remove files while going through directory
	@unlink($dir_path . $file);
}

// more important to check that all contents are present than order they are returned in
sort($contents);
var_dump($contents);

closedir($dir_handle);
?>
===DONE===
--CLEAN--
<?php
$dir_path = __DIR__ . "/私はガラスを食べられますreaddir_variation4/";
rmdir($dir_path);
?>
--EXPECTF--
*** Testing readdir() : usage variations ***

-- Iteration 1 --
resource(%d) of type (stream)
int(1)

-- Iteration 2 --
resource(%d) of type (stream)
int(1)

-- Iteration 3 --
resource(%d) of type (stream)
int(1)

-- Iteration 4 --
resource(%d) of type (stream)
int(1)

-- Iteration 5 --
resource(%d) of type (stream)
int(1)

-- Iteration 6 --
resource(%d) of type (stream)
int(1)

-- Iteration 7 --
resource(%d) of type (stream)
int(1)

-- Iteration 8 --
resource(%d) of type (stream)
int(1)

-- Iteration 9 --
resource(%d) of type (stream)
int(1)

-- Iteration 10 --
resource(%d) of type (stream)
int(1)

-- Iteration 11 --
resource(%d) of type (stream)
int(2)

-- Iteration 12 --
resource(%d) of type (stream)
int(2)

-- Iteration 13 --
resource(%d) of type (stream)
int(2)

-- Iteration 14 --
resource(%d) of type (stream)
int(2)

-- Call to readdir() --
array(16) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(45) "私はガラスを食べられます-10.5.tmp"
  [3]=>
  string(45) "私はガラスを食べられます-2345.tmp"
  [4]=>
  string(40) "私はガラスを食べられます.tmp"
  [5]=>
  string(43) "私はガラスを食べられます0.5.tmp"
  [6]=>
  string(41) "私はガラスを食べられます0.tmp"
  [7]=>
  string(53) "私はガラスを食べられます1.23456789E-9.tmp"
  [8]=>
  string(41) "私はガラスを食べられます1.tmp"
  [9]=>
  string(44) "私はガラスを食べられます10.5.tmp"
  [10]=>
  string(45) "私はガラスを食べられます12345.tmp"
  [11]=>
  string(52) "私はガラスを食べられます123456789000.tmp"
  [12]=>
  string(45) "私はガラスを食べられますArray.tmp"
  [13]=>
  string(51) "私はガラスを食べられますdouble_file.tmp"
  [14]=>
  string(47) "私はガラスを食べられますhd_file.tmp"
  [15]=>
  string(51) "私はガラスを食べられますsingle_file.tmp"
}
===DONE===
