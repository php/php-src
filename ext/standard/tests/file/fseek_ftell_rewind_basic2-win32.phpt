--TEST--
Test fseek(), ftell() & rewind() functions : basic functionality - all w and x modes
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != "WIN" )
  die("skip.. only valid for Windows");
?>
--FILE--
<?php
// include the file.inc for common functions for test
include ("file.inc");

/* Testing fseek(),ftell(),rewind() functions on all write and create with write modes */

echo "*** Testing fseek(), ftell(), rewind() : basic operations ***\n";
$file_modes = array( "w","wb","wt","w+","w+b","w+t",
                     "x","xb","xt","x+","x+b","x+t");

$file_content_types = array("text_with_new_line","alphanumeric");

$whence_set = array(SEEK_SET,SEEK_CUR,SEEK_END);
$whence_string = array("SEEK_SET", "SEEK_CUR", "SEEK_END");

$filename = __DIR__."/fseek_ftell_rewind_basic2.tmp"; // this is name of the file created by create_files()

foreach($file_content_types as $file_content_type){
  echo "\n-- File having data of type ". $file_content_type ." --\n";

  /* open the file using $files_modes and perform fseek(),ftell() and rewind() on it */
  foreach($file_modes as $file_mode) {
    echo "-- File opened in mode ".$file_mode." --\n";

    $file_handle = fopen($filename, $file_mode);
    if (!$file_handle) {
      echo "Error: failed to fopen() file: $filename!";
      exit();
    }
    $data_to_be_written="";
    fill_buffer($data_to_be_written, $file_content_type, 512); //get the data of size 512
    $data_to_be_written = $data_to_be_written;
    fwrite($file_handle,$data_to_be_written);

    // set file pointer to 0
    var_dump( rewind($file_handle) ); // set to beginning of file
    var_dump( ftell($file_handle) );

    foreach($whence_set as $whence){
      echo "-- Testing fseek() with whence = $whence_string[$whence] --\n";
      var_dump( fseek($file_handle, 10, $whence) ); //expecting int(0)
      var_dump( ftell($file_handle) ); // confirm the file pointer position
      var_dump( feof($file_handle) ); //ensure that file pointer is not at end
    } //end of whence loop

    //close the file and check the size
    fclose($file_handle);
    var_dump( filesize($filename) );

    delete_file($filename); // delete file with name
  } //end of file_mode loop
} //end of File content type loop
echo "Done\n";
?>
--EXPECT--
*** Testing fseek(), ftell(), rewind() : basic operations ***

-- File having data of type text_with_new_line --
-- File opened in mode w --
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
-- File opened in mode wb --
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
-- File opened in mode wt --
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
int(579)
bool(false)
int(569)
-- File opened in mode w+ --
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
-- File opened in mode w+b --
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
-- File opened in mode w+t --
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
int(579)
bool(false)
int(569)
-- File opened in mode x --
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
-- File opened in mode xb --
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
-- File opened in mode xt --
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
int(579)
bool(false)
int(569)
-- File opened in mode x+ --
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
-- File opened in mode x+b --
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
-- File opened in mode x+t --
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
int(579)
bool(false)
int(569)

-- File having data of type alphanumeric --
-- File opened in mode w --
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
-- File opened in mode wb --
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
-- File opened in mode wt --
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
-- File opened in mode w+ --
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
-- File opened in mode w+b --
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
-- File opened in mode w+t --
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
-- File opened in mode x --
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
-- File opened in mode xb --
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
-- File opened in mode xt --
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
-- File opened in mode x+ --
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
-- File opened in mode x+b --
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
-- File opened in mode x+t --
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
