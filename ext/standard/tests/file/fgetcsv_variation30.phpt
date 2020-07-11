--TEST--
Test fgetcsv() : usage variations - with file handle and length, file pointer pointing at end of file
--FILE--
<?php
/*
   Testing fgetcsv() to read a file whose file pointer is pointing to end of file
   and fgetcsv() provided with file handle and length arguments
*/

echo "*** Testing fgetcsv() : with file handle and length arguments, file pointer pointing at end of file ***\n";

/* the array is with two elements in it. Each element should be read as
   1st element is delimiter & 2nd element is csv fields
*/
$csv_lists = array (
  array(',', 'water,fruit'),
  array(' ', 'water fruit'),
  array(' ', '"water" "fruit"'),
  array('\\', 'water\\"fruit"\\"air"'),
  array('\\', '"water"\\"fruit"\\"""'),
);

$filename = __DIR__ . '/fgetcsv_variation30.tmp';
@unlink($filename);

$file_modes = array ("r","rb", "rt", "r+", "r+b", "r+t",
                     "a+", "a+b", "a+t",
                     "w+", "w+b", "w+t",
                     "x+", "x+b", "x+t");

$loop_counter = 1;
foreach ($csv_lists as $csv_list) {
  for($mode_counter = 0; $mode_counter < count($file_modes); $mode_counter++) {
    // create the file and add the content with has csv fields
    if ( strstr($file_modes[$mode_counter], "r") ) {
      $file_handle = fopen($filename, "w");
    } else {
      $file_handle = fopen($filename, $file_modes[$mode_counter] );
    }
    if ( !$file_handle ) {
      echo "Error: failed to create file $filename!\n";
      exit();
    }
    $delimiter = $csv_list[0];
    $csv_field = $csv_list[1];

    fwrite($file_handle, $csv_field . "\n");
    // write another line of text and a blank line
    // this will be used to test, if the fgetcsv() read more than a line and its
    // working when only a blan line is read
    fwrite($file_handle, "This is line of text without csv fields\n");
    fwrite($file_handle, "\n"); // blank line

    // close the file if the mode to be used is read mode  and re-open using read mode
    if ( strstr($file_modes[$mode_counter], "r" ) ) {
      fclose($file_handle);
      $file_handle = fopen($filename, $file_modes[$mode_counter]);
    }

    echo "\n-- Testing fgetcsv() with file opened using $file_modes[$mode_counter] mode --\n";

    // set the file pointer to EOF
    var_dump( fseek($file_handle, 0, SEEK_END) );

    // call fgetcsv() to parse csv fields

    // now file pointer should point to end of the file, try reading again
    var_dump( feof($file_handle) );
    var_dump( fgetcsv($file_handle, 1024) );
    // check the file pointer position and if eof
    var_dump( ftell($file_handle) );
    var_dump( feof($file_handle) );
    // close the file
    fclose($file_handle);
    //delete file
    unlink($filename);
  } //end of mode loop
} // end of foreach

echo "Done\n";
?>
--EXPECTF--
*** Testing fgetcsv() : with file handle and length arguments, file pointer pointing at end of file ***

-- Testing fgetcsv() with file opened using r mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using rb mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using rt mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using r+ mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using r+b mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using r+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using a+ mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using a+b mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using a+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using w+ mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using w+b mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using w+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using x+ mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using x+b mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using x+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using r mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using rb mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using rt mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using r+ mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using r+b mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using r+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using a+ mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using a+b mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using a+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using w+ mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using w+b mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using w+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using x+ mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using x+b mode --
int(0)
bool(false)
bool(false)
int(53)
bool(true)

-- Testing fgetcsv() with file opened using x+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using r mode --
int(0)
bool(false)
bool(false)
int(57)
bool(true)

-- Testing fgetcsv() with file opened using rb mode --
int(0)
bool(false)
bool(false)
int(57)
bool(true)

-- Testing fgetcsv() with file opened using rt mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using r+ mode --
int(0)
bool(false)
bool(false)
int(57)
bool(true)

-- Testing fgetcsv() with file opened using r+b mode --
int(0)
bool(false)
bool(false)
int(57)
bool(true)

-- Testing fgetcsv() with file opened using r+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using a+ mode --
int(0)
bool(false)
bool(false)
int(57)
bool(true)

-- Testing fgetcsv() with file opened using a+b mode --
int(0)
bool(false)
bool(false)
int(57)
bool(true)

-- Testing fgetcsv() with file opened using a+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using w+ mode --
int(0)
bool(false)
bool(false)
int(57)
bool(true)

-- Testing fgetcsv() with file opened using w+b mode --
int(0)
bool(false)
bool(false)
int(57)
bool(true)

-- Testing fgetcsv() with file opened using w+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using x+ mode --
int(0)
bool(false)
bool(false)
int(57)
bool(true)

-- Testing fgetcsv() with file opened using x+b mode --
int(0)
bool(false)
bool(false)
int(57)
bool(true)

-- Testing fgetcsv() with file opened using x+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using r mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using rb mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using rt mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using r+ mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using r+b mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using r+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using a+ mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using a+b mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using a+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using w+ mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using w+b mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using w+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using x+ mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using x+b mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using x+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using r mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using rb mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using rt mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using r+ mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using r+b mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using r+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using a+ mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using a+b mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using a+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using w+ mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using w+b mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using w+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)

-- Testing fgetcsv() with file opened using x+ mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using x+b mode --
int(0)
bool(false)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using x+t mode --
int(0)
bool(false)
bool(false)
int(%d)
bool(true)
Done
