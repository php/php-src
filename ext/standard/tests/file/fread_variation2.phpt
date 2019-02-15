--TEST--
Test fread() function : usage variations - read some/all chars, write only mode (Bug #42036)
--FILE--
<?php
/*
 Prototype: string fread ( resource $handle [, int $length] );
 Description: reads up to length bytes from the file pointer referenced by handle.
   Reading stops when up to length bytes have been read, EOF (end of file) is
   reached, (for network streams) when a packet becomes available, or (after
   opening userspace stream) when 8192 bytes have been read whichever comes first.
*/

/* Try reading some or all content of the file opened in write only mode */

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
  echo "\n";

  return $data_from_file;
}

echo "*** Testing fread() : usage variations ***\n";

$file_modes = array("a","ab","at",
                    "w","wb","wt",
                    "x","xb","xt" );

$file_content_types = array("numeric","text","text_with_new_line", "alphanumeric");

foreach($file_content_types as $file_content_type) {
  echo "\n-- Testing fread() with file having content of type ". $file_content_type ." --\n";

  /* open the file using $files_modes and perform fread() on it */
  foreach($file_modes as $file_mode) {
    if(!strstr($file_mode,"x")){
       /* create files with $file_content_type */
       create_files ( dirname(__FILE__), 1, $file_content_type, 0755, 1, "w", "fread_variation", 2);
    }

    $filename = dirname(__FILE__)."/fread_variation2.tmp"; // this is name of the file created by create_files()
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
    echo "-- Reading entire file content, expeceted : 0 bytes --\n";
    // read from file, by giving the file actual size,
    $data_from_file = check_read($file_handle, 1024, (strstr($file_mode, "+") ? 1024 : 0 ) );
    // calculate the hash and dump it, if data read, expecting here no data was read
    if ( $data_from_file != false)
      var_dump( md5($data_from_file) );

    // reading file by giving less than its size
    echo "-- Reading file content less than max. file size, expeceted : 0 bytes --\n";
    rewind($file_handle);
    $data_from_file = check_read($file_handle, 1000, (strstr($file_mode, "+") ? 1000 : 0 ) );
    // calculate the hash and dump it, if data read, expecting here no data was read
    if ( $data_from_file != false)
      var_dump( md5($data_from_file) );

    // now close the file
    fclose($file_handle);

    // delete the file created
    delete_file($filename); // delete file
  } // end of inner foreach loop
}// end of outer foreach loop

echo "Done\n";
?>
--EXPECT--
*** Testing fread() : usage variations ***

-- Testing fread() with file having content of type numeric --
-- File opened in mode a --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode ab --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode at --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode w --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode wb --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode wt --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode x --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode xb --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode xt --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)


-- Testing fread() with file having content of type text --
-- File opened in mode a --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode ab --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode at --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode w --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode wb --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode wt --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode x --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode xb --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode xt --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)


-- Testing fread() with file having content of type text_with_new_line --
-- File opened in mode a --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode ab --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode at --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode w --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode wb --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode wt --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode x --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode xb --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode xt --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)


-- Testing fread() with file having content of type alphanumeric --
-- File opened in mode a --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode ab --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode at --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode w --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode wb --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode wt --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode x --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode xb --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- File opened in mode xt --
-- Reading entire file content, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

-- Reading file content less than max. file size, expeceted : 0 bytes --
int(0)
bool(false)
Reading 1000 bytes from file, expecting 0 bytes ... OK
int(0)
bool(false)

Done
