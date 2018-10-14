--TEST--
Test fwrite() function : usage variations - a, ab, at, a+, a+b & a+t modes
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
 Test fwrite with file opened in mode : a,ab,at,a+,a+b,a+t
 File having content of type numeric, text,text_with_new_line & alphanumeric
*/

$file_modes = array("a","ab","at","a+","a+b","a+t");
$file_content_types = array("numeric","text","text_with_new_line","alphanumeric");


foreach($file_content_types as $file_content_type) {
  echo "\n-- Testing fwrite() with file having content of type ". $file_content_type ." --\n";

  /* open the file using $files_modes and perform fwrite() on it */
  foreach($file_modes as $file_mode) {
    echo "-- Opening file in $file_mode --\n";

    // create temp file and fill it content of type $file_content_type
    $filename = dirname(__FILE__)."/fwrite_variation3.tmp"; // this is name of the file
    create_files ( dirname(__FILE__), 1, $file_content_type, 0755, 1, "w", "fwrite_variation", 3);

    $file_handle = fopen($filename, $file_mode);
    if(!$file_handle) {
      echo "Error: failed to fopen() file: $filename!";
      exit();
    }

    $data_to_be_written="";
    fill_buffer($data_to_be_written,$file_content_type,1024);  //get the data of size 1024

    /*  Write the data into the file, verify it by checking the file pointer position, eof position,
        filesize & by displaying the content */

    // append the data to the file, starting from current position of the file pointer
    var_dump( ftell($file_handle) ); // expected: 1024
    var_dump( fwrite($file_handle,$data_to_be_written,400) );
    var_dump( ftell($file_handle) ); // expected: 1024 + 400
    var_dump( feof($file_handle) );  // expected : true

    /*overwrite data in middle of the file*/
    fseek($file_handle, SEEK_SET, (1024 + 400)/2 );
    var_dump( ftell($file_handle));  // expected: (1024 + 400)/2
    var_dump( fwrite($file_handle, $data_to_be_written, 200) );
    var_dump( ftell($file_handle) );
    var_dump( feof($file_handle) );  //Expecting bool(false)

    /* check the filesize and display file content */
    // close the file, get the size and content of the file.
    var_dump( fclose($file_handle) );
    clearstatcache();//clears file status cache
    var_dump( filesize($filename) );
    var_dump(md5(file_get_contents($filename)));
    // delete the file created
    delete_file($filename); // delete file with name fwrite_variation3.tmp
  } // end of inner foreach loop
} // end of outer foreach loop

echo "Done\n";
?>
--EXPECT--
*** Testing fwrite() various  operations ***

-- Testing fwrite() with file having content of type numeric --
-- Opening file in a --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "59ce5bf03b69069d00d6354bdc969ff6"
-- Opening file in ab --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "59ce5bf03b69069d00d6354bdc969ff6"
-- Opening file in at --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "59ce5bf03b69069d00d6354bdc969ff6"
-- Opening file in a+ --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "59ce5bf03b69069d00d6354bdc969ff6"
-- Opening file in a+b --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "59ce5bf03b69069d00d6354bdc969ff6"
-- Opening file in a+t --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "59ce5bf03b69069d00d6354bdc969ff6"

-- Testing fwrite() with file having content of type text --
-- Opening file in a --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "dbd9dffd809d82e299bc1e5c55087f3b"
-- Opening file in ab --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "dbd9dffd809d82e299bc1e5c55087f3b"
-- Opening file in at --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "dbd9dffd809d82e299bc1e5c55087f3b"
-- Opening file in a+ --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "dbd9dffd809d82e299bc1e5c55087f3b"
-- Opening file in a+b --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "dbd9dffd809d82e299bc1e5c55087f3b"
-- Opening file in a+t --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "dbd9dffd809d82e299bc1e5c55087f3b"

-- Testing fwrite() with file having content of type text_with_new_line --
-- Opening file in a --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "3f0a483fe8a2f405677844e0b1af6cf4"
-- Opening file in ab --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "3f0a483fe8a2f405677844e0b1af6cf4"
-- Opening file in at --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "3f0a483fe8a2f405677844e0b1af6cf4"
-- Opening file in a+ --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "3f0a483fe8a2f405677844e0b1af6cf4"
-- Opening file in a+b --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "3f0a483fe8a2f405677844e0b1af6cf4"
-- Opening file in a+t --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "3f0a483fe8a2f405677844e0b1af6cf4"

-- Testing fwrite() with file having content of type alphanumeric --
-- Opening file in a --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "ea0c0bfa0b10aa8e614fd33ffe295cb9"
-- Opening file in ab --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "ea0c0bfa0b10aa8e614fd33ffe295cb9"
-- Opening file in at --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "ea0c0bfa0b10aa8e614fd33ffe295cb9"
-- Opening file in a+ --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "ea0c0bfa0b10aa8e614fd33ffe295cb9"
-- Opening file in a+b --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "ea0c0bfa0b10aa8e614fd33ffe295cb9"
-- Opening file in a+t --
int(0)
int(400)
int(400)
bool(false)
int(400)
int(200)
int(600)
bool(false)
bool(true)
int(1624)
string(32) "ea0c0bfa0b10aa8e614fd33ffe295cb9"
Done
