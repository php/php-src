--TEST--
Test fscanf() function: usage variations - octal formats with integer values
--FILE--
<?php

/*
  Prototype: mixed fscanf ( resource $handle, string $format [, mixed &$...] );
  Description: Parses input from a file according to a format
*/

/* Test fscanf() to scan different integer values using different octal format types */

$file_path = __DIR__;

echo "*** Test fscanf(): different octal format types with different integer values ***\n";

// create a file
$filename = "$file_path/fscanf_variation27.tmp";
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

// different valid  integer values
$valid_ints = array(
  0,
  1,
  -1,
  -2147483648, // max negative integer value
  -2147483647,
  2147483647,  // max positive integer value
  2147483640,
  0x123B,      // integer as hexadecimal
  0x12ab,
  0Xfff,
  0XFA,
  -0x80000000, // max negative integer as hexadecimal
  0x7fffffff,  // max positive integer as hexadecimal
  0x7FFFFFFF,  // max positive integer as hexadecimal
  0123,        // integer as octal
  01,       // should be quivalent to octal 1
  -020000000000, // max negative integer as octal
  017777777777  // max positive integer as octal
);
// various octal formats
$octal_formats = array( "%o",
                        "%ho", "%lo", "%Lo",
                        " %o", "%o ", "% o",
                        "\t%o", "\n%o", "%4o",
                        "%30o", "%[0-7]", "%*o"
                 );

$counter = 1;

// writing to the file
foreach($valid_ints as $int_value) {
  @fprintf($file_handle, $int_value);
  @fprintf($file_handle, "\n");
}
// closing the file
fclose($file_handle);

// opening the file for reading
$file_handle = fopen($filename, "r");
if($file_handle == false) {
  exit("Error:failed to open file $filename");
}

$counter = 1;
// reading the values from file using different octal formats
foreach($octal_formats as $octal_format) {
  // rewind the file so that for every foreach iteration the file pointer starts from bof
  rewind($file_handle);
  echo "\n-- iteration $counter --\n";
  while( !feof($file_handle) ) {
    var_dump( fscanf($file_handle,$octal_format) );
  }
  $counter++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$filename = "$file_path/fscanf_variation27.tmp";
unlink($filename);
?>
--EXPECTF--
*** Test fscanf(): different octal format types with different integer values ***

-- iteration 1 --
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(2487)
}
array(1) {
  [0]=>
  int(319)
}
array(1) {
  [0]=>
  int(32)
}
array(1) {
  [0]=>
  int(168)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
bool(false)

-- iteration 2 --
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(2487)
}
array(1) {
  [0]=>
  int(319)
}
array(1) {
  [0]=>
  int(32)
}
array(1) {
  [0]=>
  int(168)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
bool(false)

-- iteration 3 --
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(2487)
}
array(1) {
  [0]=>
  int(319)
}
array(1) {
  [0]=>
  int(32)
}
array(1) {
  [0]=>
  int(168)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
bool(false)

-- iteration 4 --
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(2487)
}
array(1) {
  [0]=>
  int(319)
}
array(1) {
  [0]=>
  int(32)
}
array(1) {
  [0]=>
  int(168)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
bool(false)

-- iteration 5 --
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(2487)
}
array(1) {
  [0]=>
  int(319)
}
array(1) {
  [0]=>
  int(32)
}
array(1) {
  [0]=>
  int(168)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
bool(false)

-- iteration 6 --
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(2487)
}
array(1) {
  [0]=>
  int(319)
}
array(1) {
  [0]=>
  int(32)
}
array(1) {
  [0]=>
  int(168)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
bool(false)

-- iteration 7 --

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL
bool(false)

-- iteration 8 --
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(2487)
}
array(1) {
  [0]=>
  int(319)
}
array(1) {
  [0]=>
  int(32)
}
array(1) {
  [0]=>
  int(168)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
bool(false)

-- iteration 9 --
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(2487)
}
array(1) {
  [0]=>
  int(319)
}
array(1) {
  [0]=>
  int(32)
}
array(1) {
  [0]=>
  int(168)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
bool(false)

-- iteration 10 --
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-1)
}
array(1) {
  [0]=>
  int(-140)
}
array(1) {
  [0]=>
  int(-140)
}
array(1) {
  [0]=>
  int(1127)
}
array(1) {
  [0]=>
  int(1127)
}
array(1) {
  [0]=>
  int(2487)
}
array(1) {
  [0]=>
  int(319)
}
array(1) {
  [0]=>
  int(32)
}
array(1) {
  [0]=>
  int(168)
}
array(1) {
  [0]=>
  int(-140)
}
array(1) {
  [0]=>
  int(1127)
}
array(1) {
  [0]=>
  int(1127)
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-140)
}
array(1) {
  [0]=>
  int(1127)
}
bool(false)

-- iteration 11 --
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(2487)
}
array(1) {
  [0]=>
  int(319)
}
array(1) {
  [0]=>
  int(32)
}
array(1) {
  [0]=>
  int(168)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
bool(false)

-- iteration 12 --
array(1) {
  [0]=>
  string(1) "0"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(5) "21474"
}
array(1) {
  [0]=>
  string(5) "21474"
}
array(1) {
  [0]=>
  string(4) "4667"
}
array(1) {
  [0]=>
  string(3) "477"
}
array(1) {
  [0]=>
  string(2) "40"
}
array(1) {
  [0]=>
  string(3) "250"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(5) "21474"
}
array(1) {
  [0]=>
  string(5) "21474"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(5) "21474"
}
bool(false)

-- iteration 13 --
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
bool(false)

*** Done ***
