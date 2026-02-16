--TEST--
Test fscanf() function: usage variations - empty file
--FILE--
<?php

/* Test fscanf() to scan an empty file */

$file_path = __DIR__;

echo "*** Test fscanf(): to read an empty file ***\n";

// various formats
$formats = array( "%d", "%f", "%e", "%u", " %s", "%x", "%o");

$counter = 1;

// various read modes
$modes = array("r", "rb", "rt", "r+", "r+b", "r+t",
               "w+", "w+b", "w+t",
               "a+", "a+b", "a+t"
         );

$counter = 1;
// reading the values from file using different integer formats
foreach($modes as $mode) {

  // create an empty file
  $filename = "$file_path/fscanf_variation52.tmp";
  $file_handle = fopen($filename, "w");
  if($file_handle == false)
    exit("Error:failed to open file $filename");
  //closing the file
  fclose($file_handle);

  // opening file in $mode mode
  $file_handle = fopen($filename, $mode);
  if($file_handle == false) {
    exit("Error:failed to open file $filename");
  }
  echo "\n-- iteration $counter --\n";

  foreach($formats as $format) {
    var_dump( fscanf($file_handle,$format) );
    rewind($file_handle);
  }
  $counter++;
  fclose($file_handle);
  unlink($filename);
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$filename = "$file_path/fscanf_variation52.tmp";
if(file_exists($filename)) {
  unlink($filename);
}
?>
--EXPECT--
*** Test fscanf(): to read an empty file ***

-- iteration 1 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 2 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 3 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 4 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 5 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 6 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 7 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 8 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 9 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 10 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 11 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 12 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

*** Done ***
