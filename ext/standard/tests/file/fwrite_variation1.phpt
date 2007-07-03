--TEST--
Test fwrite() function : usage variations - r, rb & rt modes
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == 'WIN' ) {
   die('skip...Not valid for Windows');
}
?>
--FILE--
<?php
/*
 Prototype: int fwrite ( resource $handle,string string, [, int $length] );
 Description: fwrite() writes the contents of string to the file stream pointed to by handle.
              If the length arquement is given,writing will stop after length bytes have been
              written or the end of string reached, whichever comes first.
              fwrite() returns the number of bytes written or FALSE on error
*/


echo "*** Testing fwrite() various  operations ***\n";

// include the file.inc for Function: function delete_file($filename)
include ("file.inc");

/*
 Test fwrite with file opened in mode : r,rb,rt
 File having content of type numeric, text,text_with_new_line & alphanumeric
*/

$file_modes = array("r","rb","rt");
$file_content_types = array("numeric","text","text_with_new_line","alphanumeric");

foreach($file_content_types as $file_content_type) {
  echo "\n-- Testing fwrite() with file having content of type ". $file_content_type ." --\n";

  /* open the file using $files_modes and perform fwrite() on it */
  foreach($file_modes as $file_mode) {
    echo "-- Opening file in $file_mode --\n";
    
    // create the temp file with content of type $file_content_type
    $filename = dirname(__FILE__)."/fwrite_variation1.tmp"; // this is name of the file
    create_files ( dirname(__FILE__), 1, $file_content_type, 0755, 1, "w", "fwrite_variation");

    $file_handle = fopen($filename, $file_mode);
    if(!$file_handle) {
      echo "Error: failed to fopen() file: $filename!";
      exit();
    }

    $data_to_be_written="";
    fill_buffer($data_to_be_written,$file_content_type,1024);  //get the data of size 1024

    /*  Write the data into the file, verify it by checking the file pointer position, eof position, 
        filesize & by displaying the content */

    var_dump( ftell($file_handle) );  // expected: 0
    var_dump( fwrite($file_handle, $data_to_be_written )); 
    var_dump( ftell($file_handle) );  // expected: 0
    var_dump( feof($file_handle) );  // expected: false 
  
    // move the file pointer to end of the file and try fwrite()
    fseek($file_handle, SEEK_END, 0);
    var_dump( ftell($file_handle) );  // expecting 1024
    var_dump( fwrite($file_handle, $data_to_be_written) ); // fwrite to fail
    var_dump( ftell($file_handle) );  //check that file pointer points at eof, expected: 1024
    var_dump( feof($file_handle) );  // ensure that  feof() points to eof, expected: true

    // ensure that file content/size didn't change.
    var_dump( fclose($file_handle) );
    clearstatcache();//clears file status cache
    var_dump( filesize($filename) );  // expected: 1024
    var_dump(md5(file_get_contents($filename))); // hash the output
    delete_file($filename); // delete file with name fwrite_variation1.tmp
  } // end of inner foreach loop
} // end of outer foreach loop

echo "Done\n";
?>
--EXPECTF--
*** Testing fwrite() various  operations ***

-- Testing fwrite() with file having content of type numeric --
-- Opening file in r --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Opening file in rb --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Opening file in rt --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"

-- Testing fwrite() with file having content of type text --
-- Opening file in r --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Opening file in rb --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Opening file in rt --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "e486000c4c8452774f746a27658d87fa"

-- Testing fwrite() with file having content of type text_with_new_line --
-- Opening file in r --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Opening file in rb --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Opening file in rt --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "b09c8026a64a88d36d4c2f17983964bb"

-- Testing fwrite() with file having content of type alphanumeric --
-- Opening file in r --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
-- Opening file in rb --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
-- Opening file in rt --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
Done
