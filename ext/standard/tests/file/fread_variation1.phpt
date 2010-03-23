--TEST--
Test fread() function : usage variations - read some/all chars, read/write modes
--FILE--
<?php
/*
 Prototype: string fread ( resource $handle [, int $length] );
 Description: reads up to length bytes from the file pointer referenced by handle. 
   Reading stops when up to length bytes have been read, EOF (end of file) is 
   reached, (for network streams) when a packet becomes available, or (after 
   opening userspace stream) when 8192 bytes have been read whichever comes first.
*/

/* Read content less than file size &
   Read entire file
*/

// include the file.inc for common functions for test
include ("file.inc");

/* Function : function check_read(resource $file_handle, int $read_size, int $expect_size)
   Description : Read data from file of size $read_size and verifies that $expected_size no. of 
                 bytes are read. 
     $file_handle : File Handle
     $read_size   : No. of bytes to be read.
     $expect_size : Expected data length
   Returns: returns the data read
*/
function check_read($file_handle, $read_size, $expect_size) {
  // print file pointer position before read
  var_dump( ftell($file_handle) );
  var_dump( feof($file_handle) );

  // read the data of size $read_size
  echo "Reading $read_size bytes from file, expecting $expect_size bytes ... ";
  $data_from_file = fread($file_handle, $read_size);

  // check if data read is of expected size
  if ( strlen($data_from_file) == $expect_size)
    echo "OK\n";
  else
    echo "Error reading file, total number of bytes read = ".strlen($data_from_file)."\n";

  // file pointer position after read
  var_dump( ftell($file_handle) );
  // check if file pointer at eof()
  var_dump( feof($file_handle) );

  return $data_from_file;
}
 
echo "*** Testing fread() : usage variations ***\n";

$file_modes = array("a+","a+b","a+t",
                    "w+","w+b","w+t",
                    "x+","x+b","x+t");

$file_content_types = array("numeric","text","text_with_new_line", "alphanumeric");

foreach($file_content_types as $file_content_type) {
  echo "\n-- Testing fread() with file having content of type ". $file_content_type ." --\n";

  /* open the file using $files_modes and perform fread() on it */
  foreach($file_modes as $file_mode) {
    if(!strstr($file_mode,"x")){
       /* create files with $file_content_type */
       create_files ( dirname(__FILE__), 1, $file_content_type, 0755, 1, "w", "fread_variation");
    }
    
    $filename = dirname(__FILE__)."/fread_variation1.tmp"; // this is name of the file created by create_files()
    echo "-- File opened in mode ".$file_mode." --\n";
    $file_handle = fopen($filename, $file_mode);
    if (!$file_handle) {
       echo "Error: failed to fopen() file: $filename!";
       exit();
    }

    if(strstr($file_mode,"w") || strstr($file_mode,"x") ) {
      fill_file($file_handle, $file_content_type, 1024);
    }

    rewind($file_handle);
    echo "-- Reading entire file content, expeceted : 1024 bytes --\n";
    // read from file, by giving the file actual size,
    $data_from_file = check_read($file_handle, 1024, (strstr($file_mode, "+") ? 1024 : 1024 ) );
    // calculate the hash and dump it, if data read, expecting here no data was read
    if ( $data_from_file != false)
      var_dump( md5($data_from_file) );

    // reading file by giving less than its size
    echo "-- Reading file content less than max. file size, expeceted : 1000 bytes --\n";
    rewind($file_handle);
    $data_from_file = check_read($file_handle, 1000, (strstr($file_mode, "+") ? 1000 : 1000 ) );
    // calculate the hash and dump it, if data read, expecting here no data was read
    if ( $data_from_file != false)
      var_dump( md5($data_from_file) );

    // now close the file
    fclose($file_handle);

    // delete the file created
    delete_file($filename); // delete file
  } // end of inner foreach loop
}// end of outer foreach loop

echo"Done\n";
?>
--EXPECTF--
*** Testing fread() : usage variations ***

-- Testing fread() with file having content of type numeric --
-- File opened in mode a+ --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
-- File opened in mode a+b --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
-- File opened in mode a+t --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
-- File opened in mode w+ --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
-- File opened in mode w+b --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
-- File opened in mode w+t --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
-- File opened in mode x+ --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
-- File opened in mode x+b --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
-- File opened in mode x+t --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"

-- Testing fread() with file having content of type text --
-- File opened in mode a+ --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
-- File opened in mode a+b --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
-- File opened in mode a+t --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
-- File opened in mode w+ --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
-- File opened in mode w+b --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
-- File opened in mode w+t --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
-- File opened in mode x+ --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
-- File opened in mode x+b --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
-- File opened in mode x+t --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"

-- Testing fread() with file having content of type text_with_new_line --
-- File opened in mode a+ --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
-- File opened in mode a+b --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
-- File opened in mode a+t --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
-- File opened in mode w+ --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
-- File opened in mode w+b --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
-- File opened in mode w+t --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
-- File opened in mode x+ --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
-- File opened in mode x+b --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
-- File opened in mode x+t --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"

-- Testing fread() with file having content of type alphanumeric --
-- File opened in mode a+ --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
-- File opened in mode a+b --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
-- File opened in mode a+t --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
-- File opened in mode w+ --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
-- File opened in mode w+b --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
-- File opened in mode w+t --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
-- File opened in mode x+ --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
-- File opened in mode x+b --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
-- File opened in mode x+t --
-- Reading entire file content, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
-- Reading file content less than max. file size, expeceted : 1000 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
Done