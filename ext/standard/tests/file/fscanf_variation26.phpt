--TEST--
Test fscanf() function: usage variations - char formats with chars
--FILE--
<?php

/* Test fscanf() to scan different chars using different char format types */

$file_path = __DIR__;

echo "*** Test fscanf(): different char format types with chars ***\n";

// create a file
$filename = "$file_path/fscanf_variation26.tmp";
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

// array of chars
$char_types = array( 'a', "a", 67, -67, 99 );

$char_formats = array( "%c",
               "%hc", "%lc", "%Lc",
               " %c", "%c ", "% c",
               "\t%c", "\n%c", "%4c",
               "%30c", "%[a-zA-Z@#$&0-9]", "%*c");

$counter = 1;

// writing to the file
foreach($char_types as $char) {
  @fprintf($file_handle, $char);
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
// reading the values from file using different char formats
foreach($char_formats as $char_format) {
  // rewind the file so that for every foreach iteration the file pointer starts from bof
  rewind($file_handle);
  echo "\n-- iteration $counter --\n";
  while( !feof($file_handle) ) {
    try {
      var_dump(fscanf($file_handle,$char_format));
    } catch (ValueError $exception) {
      echo $exception->getMessage() . "\n";
    }
  }
  $counter++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$filename = "$file_path/fscanf_variation26.tmp";
unlink($filename);
?>
--EXPECT--
*** Test fscanf(): different char format types with chars ***

-- iteration 1 --
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "6"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "9"
}
bool(false)

-- iteration 2 --
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "6"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "9"
}
bool(false)

-- iteration 3 --
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "6"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "9"
}
bool(false)

-- iteration 4 --
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "6"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "9"
}
bool(false)

-- iteration 5 --
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "6"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "9"
}
bool(false)

-- iteration 6 --
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "6"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "9"
}
bool(false)

-- iteration 7 --
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
bool(false)

-- iteration 8 --
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "6"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "9"
}
bool(false)

-- iteration 9 --
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "6"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "9"
}
bool(false)

-- iteration 10 --
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(2) "67"
}
array(1) {
  [0]=>
  string(3) "-67"
}
array(1) {
  [0]=>
  string(2) "99"
}
bool(false)

-- iteration 11 --
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(2) "67"
}
array(1) {
  [0]=>
  string(3) "-67"
}
array(1) {
  [0]=>
  string(2) "99"
}
bool(false)

-- iteration 12 --
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(2) "67"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(2) "99"
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
bool(false)

*** Done ***
