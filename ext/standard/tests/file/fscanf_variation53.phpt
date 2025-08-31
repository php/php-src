--TEST--
Test fscanf() function: usage variations - file pointer pointing to EOF
--FILE--
<?php

/* Test fscanf() to read a file when file pointer is pointing to EOF */

$file_path = __DIR__;

echo "*** Test fscanf(): to read a file when file pointer is pointing to EOF ***\n";

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
  $filename = "$file_path/fscanf_variation53.tmp";
  $file_handle = fopen($filename, "w");
  if($file_handle == false)
    exit("Error:failed to open file $filename");

  //writing data to the file
  @fwrite($file_handle, "Sample text\n");

  // writing a blank line
  @fwrite($file_handle, "\n");

  //closing the file
  fclose($file_handle);

  // opening file in $mode mode
  $file_handle = fopen($filename, $mode);
  if($file_handle == false) {
    exit("Error:failed to open file $filename");
  }
  echo "\n-- iteration $counter --\n";

  // current location
  var_dump( ftell($file_handle) );

  // set the file pointer to eof
  var_dump( fseek($file_handle, 0, SEEK_END) );

  // current location
  var_dump( ftell($file_handle) );

  foreach($formats as $format) {
    var_dump( fscanf($file_handle,$format) );
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
$filename = "$file_path/fscanf_variation53.tmp";
if(file_exists($filename)) {
  unlink($filename);
}
?>
--EXPECT--
*** Test fscanf(): to read a file when file pointer is pointing to EOF ***

-- iteration 1 --
int(0)
int(0)
int(13)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 2 --
int(0)
int(0)
int(13)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 3 --
int(0)
int(0)
int(13)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 4 --
int(0)
int(0)
int(13)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 5 --
int(0)
int(0)
int(13)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 6 --
int(0)
int(0)
int(13)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 7 --
int(0)
int(0)
int(0)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 8 --
int(0)
int(0)
int(0)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 9 --
int(0)
int(0)
int(0)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 10 --
int(0)
int(0)
int(13)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 11 --
int(0)
int(0)
int(13)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

-- iteration 12 --
int(0)
int(0)
int(13)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

*** Done ***
