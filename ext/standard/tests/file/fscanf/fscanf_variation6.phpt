--TEST--
Test fscanf() function: usage variations - integer formats with strings
--FILE--
<?php

/* Test fscanf() to scan strings using different integer format types */

$file_path = __DIR__;

echo "*** Test fscanf(): different integer format types with strings ***\n";

// create a file
$filename = "$file_path/fscanf_variation6.tmp";
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

// array of strings
$strings = [
  '',
  '0',
  '1',
  "\01",
  '\01',
  'string',
  'true',
  'false',
  'null'
];

$int_formats = array( "%d", "%hd", "%ld", "%Ld", " %d", "%d ", "\t%d", "\n%d", "%4d", "%30d", "%[0-9]", "%*d");

$counter = 1;

// writing to the file
foreach($strings as $string) {
  @fprintf($file_handle, $string);
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
// reading the values from file using different integer formats
foreach($int_formats as $int_format) {
  // rewind the file so that for every foreach iteration the file pointer starts from bof
  rewind($file_handle);
  echo "\n-- iteration $counter --\n";
  while( !feof($file_handle) ) {
    try {
      var_dump(fscanf($file_handle,$int_format));
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
$filename = "$file_path/fscanf_variation6.tmp";
unlink($filename);
?>
--EXPECT--
*** Test fscanf(): different integer format types with strings ***

-- iteration 1 --
NULL
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

-- iteration 2 --
NULL
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

-- iteration 3 --
NULL
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

-- iteration 4 --
NULL
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

-- iteration 5 --
NULL
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

-- iteration 6 --
NULL
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

-- iteration 7 --
NULL
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

-- iteration 8 --
NULL
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

-- iteration 9 --
NULL
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

-- iteration 10 --
NULL
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

-- iteration 11 --
array(1) {
  [0]=>
  NULL
}
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

-- iteration 12 --
NULL
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
