--TEST--
Test fscanf() function: usage variations - hexa formats with arrays 
--FILE--
<?php

/*
  Prototype: mixed fscanf ( resource $handle, string $format [, mixed &$...] );
  Description: Parses input from a file according to a format
*/

/* Test fscanf() to scan arrays using different hexa format types */

$file_path = dirname(__FILE__);

echo "*** Test fscanf(): different hexa format types with arrays ***\n"; 

// create a file
$filename = "$file_path/fscanf_variation36.tmp";
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

// array types
$array_types = array (
  array(),
  array(0),
  array(1),
  array(NULL),
  array(null),
  array("string"),
  array(true),
  array(TRUE),
  array(false),
  array(FALSE),
  array(1,2,3,4),
  array(1 => "One", "two" => 2)
);

$hexa_formats = array( "%x", "%hx", "%lx", "%Lx", " %x", "%x ", "% x", "\t%x", "\n%x", "%4x", "%30x", "%[0-9]", "%*x");

$counter = 1;

// writing to the file
foreach($array_types as $value) {
  @fprintf($file_handle, $value);
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
// reading the values from file using different hexa formats
foreach($hexa_formats as $hexa_format) {
  // rewind the file so that for every foreach iteration the file pointer starts from bof
  rewind($file_handle);
  echo "\n-- iteration $counter --\n";
  while( !feof($file_handle) ) {
    var_dump( fscanf($file_handle,$hexa_format) );
  }
  $counter++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$filename = "$file_path/fscanf_variation36.tmp";
unlink($filename);
?>
--EXPECTF--
*** Test fscanf(): different hexa format types with arrays ***

-- iteration 1 --
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
bool(false)

-- iteration 2 --
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
bool(false)

-- iteration 3 --
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
bool(false)

-- iteration 4 --
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
bool(false)

-- iteration 5 --
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
bool(false)

-- iteration 6 --
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
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
bool(false)

-- iteration 8 --
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
bool(false)

-- iteration 9 --
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
bool(false)

-- iteration 10 --
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
bool(false)

-- iteration 11 --
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(10)
}
bool(false)

-- iteration 12 --
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
bool(false)

*** Done ***
