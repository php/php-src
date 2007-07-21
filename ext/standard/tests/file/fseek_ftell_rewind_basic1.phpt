--TEST--
Test fseek(), ftell() & rewind() functions : Basic functionality - all r and a modes
--FILE--
<?php
/* Prototype: int fseek ( resource $handle, int $offset [, int $whence] );
   Description: Seeks on a file pointer

   Prototype: bool rewind ( resource $handle );
   Description: Rewind the position of a file pointer

   Prototype: int ftell ( resource $handle );
   Description: Tells file pointer read/write position
*/

// include the file.inc for common functions for test
include ("file.inc");

/* Testing fseek(),ftell(),rewind() functions on all read and append modes */
echo "*** Testing fseek(), ftell(), rewind() : basic operations ***\n";
$file_modes = array( "r","rb","rt","r+","r+b","r+t",
                     "a","ab","at","a+","a+b","a+t");
$file_content_types = array( "text_with_new_line","alphanumeric");

$whence_set = array(SEEK_SET,SEEK_CUR,SEEK_END);
$whence_string = array("SEEK_SET", "SEEK_CUR", "SEEK_END");

$filename = dirname(__FILE__)."/fseek_ftell_rewind_basic1.tmp"; // this is name of the file created by create_files()
  /* open the file using $files_modes and perform fseek(),ftell() and rewind() on it */
foreach($file_content_types as $file_content_type){
  echo "\n-- File having data of type ". $file_content_type ." --\n";

  foreach($file_modes as $file_mode) {
    echo "-- File opened in mode ".$file_mode." --\n";

    create_files ( dirname(__FILE__), 1, $file_content_type, 0755, 512, "w", "fseek_ftell_rewind_basic"
                      ,1,"bytes",".tmp"); //create a file with 512 bytes size
    $file_handle = fopen($filename, $file_mode);
    if (!$file_handle) {
      echo "Error: failed to fopen() file: $filename!";
      exit();
    }
    // set the file pointer to 0
    var_dump( rewind($file_handle) ); // Confirm file pointer moves correctly
    var_dump( ftell($file_handle) ); // confirm the file pointer position
 
    foreach($whence_set as $whence){
      echo "-- Testing fseek() with whence = $whence_string[$whence] --\n";
      var_dump( fseek($file_handle,10,$whence) ); //expecting int(0)
      var_dump( ftell($file_handle) ); // confirm the file pointer position
      var_dump( feof($file_handle) ); //ensure that file pointer is not at end
    } //end of whence loop

    //close the file and check the size
    fclose($file_handle);
    var_dump( filesize($filename) );

    delete_file($filename); // delete file with name
  } //end of file_modes loop
} //end of file_content_types loop

echo "Done\n";
?>
--EXPECTF--
*** Testing fseek(), ftell(), rewind() : basic operations ***

-- File having data of type text_with_new_line --
-- File opened in mode r --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode rb --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode rt --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode r+ --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode r+b --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode r+t --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode a --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode ab --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode at --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode a+ --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode a+b --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode a+t --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)

-- File having data of type alphanumeric --
-- File opened in mode r --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode rb --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode rt --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode r+ --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode r+b --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode r+t --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode a --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode ab --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode at --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode a+ --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode a+b --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
-- File opened in mode a+t --
bool(true)
int(0)
-- Testing fseek() with whence = SEEK_SET --
int(0)
int(10)
bool(false)
-- Testing fseek() with whence = SEEK_CUR --
int(0)
int(20)
bool(false)
-- Testing fseek() with whence = SEEK_END --
int(0)
int(522)
bool(false)
int(512)
Done