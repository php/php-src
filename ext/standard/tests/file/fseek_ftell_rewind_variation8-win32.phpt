--TEST--
Test fseek(), ftell() & rewind() functions : usage variations - all w and x modes, SEEK_END
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != "WIN" )
  die("skip.. only valid for Windows");
?>
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

/* Testing fseek(),ftell(),rewind() functions
     1. All  write and create with write modes
     2. Testing fseek() with whence = SEEK_END
*/
echo "*** Testing fseek(), ftell(), rewind() : whence = SEEK_END & all r and a modes ***\n";

$file_modes = array( "w","wb","wt","w+","w+b","w+t",
                     "x","xb","xt","x+","x+b","x+t");
$file_content_types = array( "text_with_new_line","alphanumeric");

$offset = array(-1,0,1,512,600); // different offsets

$filename = __DIR__."/fseek_ftell_rewind_variation8.tmp"; // this is name of the file created by create_files()

/* open the file using $files_modes and perform fseek(),ftell() and rewind() on it */
foreach($file_content_types as $file_content_type){
  echo "\n-- File having data of type ". $file_content_type ." --\n";

  foreach($file_modes as $file_mode) {
    echo "-- File opened in mode ".$file_mode." --\n";
    $file_handle = fopen($filename, $file_mode);
    if (!$file_handle){
      echo "Error: failed to fopen() file: $filename!";
      exit();
    }
    $data_to_be_written="";
    fill_buffer($data_to_be_written, $file_content_type, 512); //get the data of size 512
    $data_to_be_written = $data_to_be_written;
    fwrite($file_handle,$data_to_be_written);
    rewind($file_handle);

    foreach($offset as $count){
      var_dump( fseek($file_handle,$count,SEEK_END) );
      var_dump( ftell($file_handle) ); // confirm the file pointer position
      var_dump( feof($file_handle) ); //ensure that file pointer is not at end
    } //end of offset loop

    //close the file and check the size
    fclose($file_handle);
    var_dump( filesize($filename) );

    delete_file($filename); // delete file with name
  } //end of file_mode loop
} //end of file_content_types loop

echo "Done\n";
?>
--EXPECT--
*** Testing fseek(), ftell(), rewind() : whence = SEEK_END & all r and a modes ***

-- File having data of type text_with_new_line --
-- File opened in mode w --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode wb --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode wt --
int(0)
int(568)
bool(false)
int(0)
int(569)
bool(false)
int(0)
int(570)
bool(false)
int(0)
int(1081)
bool(false)
int(0)
int(1169)
bool(false)
int(569)
-- File opened in mode w+ --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode w+b --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode w+t --
int(0)
int(568)
bool(false)
int(0)
int(569)
bool(false)
int(0)
int(570)
bool(false)
int(0)
int(1081)
bool(false)
int(0)
int(1169)
bool(false)
int(569)
-- File opened in mode x --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode xb --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode xt --
int(0)
int(568)
bool(false)
int(0)
int(569)
bool(false)
int(0)
int(570)
bool(false)
int(0)
int(1081)
bool(false)
int(0)
int(1169)
bool(false)
int(569)
-- File opened in mode x+ --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode x+b --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode x+t --
int(0)
int(568)
bool(false)
int(0)
int(569)
bool(false)
int(0)
int(570)
bool(false)
int(0)
int(1081)
bool(false)
int(0)
int(1169)
bool(false)
int(569)

-- File having data of type alphanumeric --
-- File opened in mode w --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode wb --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode wt --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode w+ --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode w+b --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode w+t --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode x --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode xb --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode xt --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode x+ --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode x+b --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
-- File opened in mode x+t --
int(0)
int(511)
bool(false)
int(0)
int(512)
bool(false)
int(0)
int(513)
bool(false)
int(0)
int(1024)
bool(false)
int(0)
int(1112)
bool(false)
int(512)
Done
