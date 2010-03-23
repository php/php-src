--TEST--
Test fwrite() function : usage variations - r+, r+b & r+t modes
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != 'WIN' ) {
   die('skip...Not valid for Linux');
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
 Test fwrite with file opened in mode : r+,r+b,r+t
 File having content of type numeric, text,text_with_new_line & alphanumeric
*/

$file_modes = array("r+", "r+b", "r+t");
$file_content_types = array("numeric","text","text_with_new_line","alphanumeric");


foreach($file_content_types as $file_content_type) {
  echo "\n-- Testing fwrite() with file having content of type ". $file_content_type ." --\n";

  /* open the file using $files_modes and perform fwrite() on it */
  foreach($file_modes as $file_mode) {
    echo "-- Opening file in $file_mode --\n";

    // create temp file and fill the data of type $file_content_type
    $filename = dirname(__FILE__)."/fwrite_variation2.tmp"; // this is name of the file
    create_files ( dirname(__FILE__), 1, $file_content_type, 0755, 1, "w", "fwrite_variation", 2);

    $file_handle = fopen($filename, $file_mode);
    if(!$file_handle) {
      echo "Error: failed to fopen() file: $filename!";
      exit();
    }

    $data_to_be_written="";
    fill_buffer($data_to_be_written,$file_content_type,1024);  //get the data of size 1024

    /*  Write the data into the file, verify it by checking the file pointer position, eof position, 
        filesize & by displaying the content */

    /*overwrite first 400 bytes in the file*/
    var_dump( ftell($file_handle) );  // expected : 0
    var_dump( fwrite($file_handle, $data_to_be_written, 400));
    var_dump( ftell($file_handle) );  // expected: 400
    var_dump( feof($file_handle) );  //Expecting bool(false)

    /*overwrite data in middle of the file*/
    fseek($file_handle, SEEK_SET, 1024/2 );
    var_dump( ftell($file_handle));  // expected: 1024/2
    var_dump( fwrite($file_handle, $data_to_be_written, 200) );
    var_dump( ftell($file_handle) );
    var_dump( feof($file_handle) );  //Expecting bool(false)

    /* write at the end of the file */
    fseek($file_handle, SEEK_END, 0);
    var_dump( ftell($file_handle) );  // expected: 1024
    var_dump( feof($file_handle) );
    var_dump( fwrite($file_handle, $data_to_be_written, 200) );
    var_dump( ftell($file_handle) );
    var_dump( feof($file_handle) );  //Expecting bool(false)

    /* display the file content, check the file size  */
    var_dump( fclose($file_handle) );
    clearstatcache();//clears file status cache
    var_dump( filesize($filename) );
    var_dump(md5(file_get_contents($filename)));
    delete_file($filename); // delete file with name fwrite_variation2.tmp

  } // end of inner foreach loop
} // end of outer foreach loop

echo "Done\n";
?>
--EXPECTF--
*** Testing fwrite() various  operations ***

-- Testing fwrite() with file having content of type numeric --
-- Opening file in r+ --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
int(2)
bool(false)
int(200)
int(202)
bool(false)
bool(true)
int(1024)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Opening file in r+b --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
int(2)
bool(false)
int(200)
int(202)
bool(false)
bool(true)
int(1024)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Opening file in r+t --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
int(2)
bool(false)
int(200)
int(202)
bool(false)
bool(true)
int(1024)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"

-- Testing fwrite() with file having content of type text --
-- Opening file in r+ --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
int(2)
bool(false)
int(200)
int(202)
bool(false)
bool(true)
int(1024)
string(32) "3bdaf80dae28bc24bb304daa5ffee16c"
-- Opening file in r+b --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
int(2)
bool(false)
int(200)
int(202)
bool(false)
bool(true)
int(1024)
string(32) "3bdaf80dae28bc24bb304daa5ffee16c"
-- Opening file in r+t --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
int(2)
bool(false)
int(200)
int(202)
bool(false)
bool(true)
int(1024)
string(32) "3bdaf80dae28bc24bb304daa5ffee16c"

-- Testing fwrite() with file having content of type text_with_new_line --
-- Opening file in r+ --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
int(2)
bool(false)
int(200)
int(202)
bool(false)
bool(true)
int(1024)
string(32) "b188d7c8aa229cbef067e5970f2daba9"
-- Opening file in r+b --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
int(2)
bool(false)
int(200)
int(202)
bool(false)
bool(true)
int(1024)
string(32) "b188d7c8aa229cbef067e5970f2daba9"
-- Opening file in r+t --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
int(2)
bool(false)
int(200)
int(202)
bool(false)
bool(true)
int(1024)
string(32) "991652c76db8d17c790c702ac0a6dc5f"

-- Testing fwrite() with file having content of type alphanumeric --
-- Opening file in r+ --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
int(2)
bool(false)
int(200)
int(202)
bool(false)
bool(true)
int(1024)
string(32) "5d4ec23a3d9dd447e2f702d9e0e114d9"
-- Opening file in r+b --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
int(2)
bool(false)
int(200)
int(202)
bool(false)
bool(true)
int(1024)
string(32) "5d4ec23a3d9dd447e2f702d9e0e114d9"
-- Opening file in r+t --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
int(2)
bool(false)
int(200)
int(202)
bool(false)
bool(true)
int(1024)
string(32) "5d4ec23a3d9dd447e2f702d9e0e114d9"
Done
