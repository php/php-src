--TEST--
Test fgetc() function : usage variations - read when file pointer at EOF
--FILE--
<?php
/*
 Prototype: string fgetc ( resource $handle );
 Description: Gets character from file pointer
*/
// include the header for common test function
include ("file.inc");

echo "*** Testing fgetc() : usage variations ***\n";
echo "-- Testing fgetc() with file whose file pointer is pointing to EOF --\n";
// create a file
create_files(dirname(__FILE__), 1, "text_with_new_line", 0755, 1, "w", "fgetc_variation");

$filename = dirname(__FILE__)."/fgetc_variation1.tmp";

// loop to check the file opened in different read modes
$file_modes = array("r", "rb", "rt", "r+", "r+b", "r+t");
$loop_counter =0;
for(; $loop_counter < count($file_modes); $loop_counter++) {
  // print the hearder
  echo "-- File opened in mode : $file_modes[$loop_counter] --\n";
  // open the file
  $file_handle = fopen ($filename, $file_modes[$loop_counter]);
  if (!$file_handle) {
    echo "Error: failed to open file $filename! \n";
    exit();
  }

  // seek to end of the file and try fgetc()
  var_dump( fseek($file_handle, 0, SEEK_END) ); // set file pointer to eof
  var_dump( feof($file_handle) );  // expected false
  var_dump( ftell($file_handle) );  // ensure that file pointer is at eof
  var_dump( fgetc($file_handle) ); // try n read a char, none expected
  var_dump( feof($file_handle) ); // ensure that file pointer is at eof
  var_dump( ftell($file_handle) ); // file pointer position

  // close the file handle
  fclose($file_handle);
}
echo "Done\n";
?>
--CLEAN--
<?php
unlink( dirname(__FILE__)."/fgetc_variation1.tmp");
?>
--EXPECT--
*** Testing fgetc() : usage variations ***
-- Testing fgetc() with file whose file pointer is pointing to EOF --
-- File opened in mode : r --
int(0)
bool(false)
int(1024)
bool(false)
bool(true)
int(1024)
-- File opened in mode : rb --
int(0)
bool(false)
int(1024)
bool(false)
bool(true)
int(1024)
-- File opened in mode : rt --
int(0)
bool(false)
int(1024)
bool(false)
bool(true)
int(1024)
-- File opened in mode : r+ --
int(0)
bool(false)
int(1024)
bool(false)
bool(true)
int(1024)
-- File opened in mode : r+b --
int(0)
bool(false)
int(1024)
bool(false)
bool(true)
int(1024)
-- File opened in mode : r+t --
int(0)
bool(false)
int(1024)
bool(false)
bool(true)
int(1024)
Done
