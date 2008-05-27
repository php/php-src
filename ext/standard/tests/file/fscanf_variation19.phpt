--TEST--
Test fscanf() function: usage variations - string formats with boolean 
--FILE--
<?php

/*
  Prototype: mixed fscanf ( resource $handle, string $format [, mixed &$...] );
  Description: Parses input from a file according to a format
*/

/* Test fscanf() to scan boolean data using different string format types */

$file_path = dirname(__FILE__);

echo "*** Test fscanf(): different string format types with boolean data ***\n"; 

// create a file
$filename = "$file_path/fscanf_variation19.tmp";
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

// array of boolean types
$bool_types = array (
  true,
  false,
  TRUE,
  FALSE,
);

$string_formats = array( "%s",
                         "%hs", "%ls", "%Ls",
                         " %s", "%s ", "% s",
			 "\t%s", "\n%s", "%4s",
			 "%30s", "%[a-zA-Z0-9]", "%*s");

$counter = 1;

// writing to the file
foreach($bool_types as $value) {
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
// reading the values from file using different string formats
foreach($string_formats as $string_format) {
  // rewind the file so that for every foreach iteration the file pointer starts from bof
  rewind($file_handle);
  echo "\n-- iteration $counter --\n";
  while( !feof($file_handle) ) {
    var_dump( fscanf($file_handle,$string_format) );
  }
  $counter++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$filename = "$file_path/fscanf_variation19.tmp";
unlink($filename);
?>
--EXPECTF--
*** Test fscanf(): different string format types with boolean data ***

-- iteration 1 --
array(1) {
  [0]=>
  string(1) "1"
}
NULL
array(1) {
  [0]=>
  string(1) "1"
}
NULL
bool(false)

-- iteration 2 --
array(1) {
  [0]=>
  string(1) "1"
}
NULL
array(1) {
  [0]=>
  string(1) "1"
}
NULL
bool(false)

-- iteration 3 --
array(1) {
  [0]=>
  string(1) "1"
}
NULL
array(1) {
  [0]=>
  string(1) "1"
}
NULL
bool(false)

-- iteration 4 --
array(1) {
  [0]=>
  string(1) "1"
}
NULL
array(1) {
  [0]=>
  string(1) "1"
}
NULL
bool(false)

-- iteration 5 --
array(1) {
  [0]=>
  string(1) "1"
}
NULL
array(1) {
  [0]=>
  string(1) "1"
}
NULL
bool(false)

-- iteration 6 --
array(1) {
  [0]=>
  string(1) "1"
}
NULL
array(1) {
  [0]=>
  string(1) "1"
}
NULL
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
bool(false)

-- iteration 8 --
array(1) {
  [0]=>
  string(1) "1"
}
NULL
array(1) {
  [0]=>
  string(1) "1"
}
NULL
bool(false)

-- iteration 9 --
array(1) {
  [0]=>
  string(1) "1"
}
NULL
array(1) {
  [0]=>
  string(1) "1"
}
NULL
bool(false)

-- iteration 10 --
array(1) {
  [0]=>
  string(1) "1"
}
NULL
array(1) {
  [0]=>
  string(1) "1"
}
NULL
bool(false)

-- iteration 11 --
array(1) {
  [0]=>
  string(1) "1"
}
NULL
array(1) {
  [0]=>
  string(1) "1"
}
NULL
bool(false)

-- iteration 12 --
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
  string(1) "1"
}
array(1) {
  [0]=>
  NULL
}
bool(false)

-- iteration 13 --
array(0) {
}
NULL
array(0) {
}
NULL
bool(false)

*** Done ***
