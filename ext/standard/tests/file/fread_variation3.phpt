--TEST--
Test fread() function : usage variations - read beyond file size, read/write mode
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
  die('skip...Not valid for Windows');
}
?>
--FILE--
<?php
/*
 Prototype: string fread ( resource $handle [, int $length] );
 Description: reads up to length bytes from the file pointer referenced by handle.
   Reading stops when up to length bytes have been read, EOF (end of file) is
   reached, (for network streams) when a packet becomes available, or (after
   opening userspace stream) when 8192 bytes have been read whichever comes first.
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

$file_content_types = array("numeric","text","text_with_new_line");

foreach($file_content_types as $file_content_type) {
  echo "\n-- Testing fread() with file having content of type ". $file_content_type ." --\n";

  /* open the file using $files_modes and perform fread() on it */
  foreach($file_modes as $file_mode) {
    if(!strstr($file_mode,"x")){
       /* create files with $file_content_type */
       create_files ( dirname(__FILE__), 1, $file_content_type, 0755, 1, "w", "fread_variation", 3);
    }

    $filename = dirname(__FILE__)."/fread_variation3.tmp"; // this is name of the file created by create_files()
    echo "-- File opened in mode ".$file_mode." --\n";
    $file_handle = fopen($filename, $file_mode);
    if (!$file_handle) {
       echo "Error: failed to fopen() file: $filename!";
       exit();
    }

    if(strstr($file_mode,"w") || strstr($file_mode,"x") ) {
      $data_to_be_written="";
      fill_file($file_handle, $file_content_type, 1024);
    }

    rewind($file_handle);

    // read file by giving size more than its size
    echo "-- Reading beyond filesize, expeceted : 1024 bytes --\n";
    rewind($file_handle);
    $data_from_file = check_read($file_handle, 1030, ( strstr($file_mode, "+") ? 1024 : 1024) );
    if ( $data_from_file != false)
      var_dump( md5($data_from_file) );

    rewind($file_handle);
    echo "-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --\n";
    // try fread when file pointer at end
    fseek($file_handle, 0, SEEK_END);
    //reading file when file pointer at end
    $data_from_file = check_read($file_handle, 10, 0);
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
--EXPECT--
*** Testing fread() : usage variations ***

-- Testing fread() with file having content of type numeric --
-- File opened in mode a+ --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode a+b --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode a+t --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode w+ --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode w+b --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode w+t --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode x+ --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode x+b --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode x+t --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)

-- Testing fread() with file having content of type text --
-- File opened in mode a+ --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode a+b --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode a+t --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode w+ --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode w+b --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode w+t --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode x+ --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode x+b --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode x+t --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)

-- Testing fread() with file having content of type text_with_new_line --
-- File opened in mode a+ --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode a+b --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode a+t --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode w+ --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode w+b --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode w+t --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode x+ --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode x+b --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
-- File opened in mode x+t --
-- Reading beyond filesize, expeceted : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Reading beyond filesize when file pointer pointing to EOF, expeceted : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... OK
int(1024)
bool(true)
Done
