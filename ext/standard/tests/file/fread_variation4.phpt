--TEST--
Test fread() function : usage variations - read beyond file size, write only mode
--FILE--
<?php
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

$file_modes = array("a","ab","at",
                    "w","wb","wt",
                    "x","xb","xt" );

$file_content_types = array("numeric","text","text_with_new_line");

foreach($file_content_types as $file_content_type) {
  echo "\n-- Testing fread() with file having content of type ". $file_content_type ." --\n";

  /* open the file using $files_modes and perform fread() on it */
  foreach($file_modes as $file_mode) {
    if(!strstr($file_mode,"x")){
       /* create files with $file_content_type */
       create_files ( __DIR__, 1, $file_content_type, 0755, 1, "w", "fread_variation", 4);
    }

    $filename = __DIR__."/fread_variation4.tmp"; // this is name of the file created by create_files()
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
    echo "-- Reading beyond filesize, expected : 1024 bytes --\n";
    // read file by giving size more than its size
    rewind($file_handle);
    $data_from_file = check_read($file_handle, 1030, ( strstr($file_mode, "+") ? 1024 : 0) );
    if ( $data_from_file != false)
      var_dump( md5($data_from_file) );

    echo "-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --\n";
    rewind($file_handle);
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
--EXPECTF--
*** Testing fread() : usage variations ***

-- Testing fread() with file having content of type numeric --
-- File opened in mode a --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode ab --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode at --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode w --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode wb --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode wt --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode x --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode xb --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode xt --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)

-- Testing fread() with file having content of type text --
-- File opened in mode a --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode ab --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode at --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode w --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode wb --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode wt --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode x --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode xb --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode xt --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)

-- Testing fread() with file having content of type text_with_new_line --
-- File opened in mode a --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode ab --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode at --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode w --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode wb --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode wt --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(%r1024|1137%r)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(%r1024|1137%r)
bool(false)
-- File opened in mode x --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode xb --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(1024)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(1024)
bool(false)
-- File opened in mode xt --
-- Reading beyond filesize, expected : 1024 bytes --
int(0)
bool(false)
Reading 1030 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(0)
bool(false)
-- Reading beyond filesize when file pointer pointing to EOF, expected : 0 bytes --
int(%r1024|1137%r)
bool(false)
Reading 10 bytes from file, expecting 0 bytes ... 
Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
OK
int(%r1024|1137%r)
bool(false)
Done
