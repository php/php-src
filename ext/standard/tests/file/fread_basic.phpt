--TEST--
Test fread() function : basic functionality
--FILE--
<?php
// include the file.inc for common functions for test
include ("file.inc");

/* Function : function check_size(string $data, int $expect_size)
   Description : Check the length of the data, and compare the size with $expect_size
     $data : Text data.
     $expect_size : Expected data length
*/
function check_size($data, $expect_size) {

  $size=strlen($data);
  if ( $size == $expect_size)
    echo "OK\n";
  else
    echo "Error: Expected: $expect_size, Actual: $size";
 }


echo "*** Testing fread() basic operations ***\n";
/*
 test fread with file opened in "r" and "rb" mode only
 Content with numeric and strings with it
*/
$file_modes = array( "r", "rb", "rt", "r+", "r+b", "r+t");
$file_content_types = array("numeric","text","text_with_new_line","alphanumeric");

 foreach($file_content_types as $file_content_type) {
   echo "\n-- Testing fread) with file having data of type ". $file_content_type ." --\n";
   /* create files with $file_content_type */
   create_files ( __DIR__, 1, $file_content_type, 0755, 1, "w", "fread_basic");
   $filename = __DIR__."/fread_basic1.tmp"; // this is name of the file created by create_files()

  /* open the file using $files_modes and perform fread() on it */
   for($inner_loop_counter = 0;
       $inner_loop_counter < count($file_modes);
       $inner_loop_counter++) {

    echo "-- File opened in mode ".$file_modes[$inner_loop_counter]." --\n";
    $file_handle = fopen($filename, $file_modes[$inner_loop_counter]);
    if (!$file_handle) {
       echo "Error: failed to fopen() file: $filename!";
       exit();
    }

    /* read file by giving the actual length, check the length and content by calculating the
      hash using md5() function
    */
    /* Reading 1024 bytes from file, expecting 1024 bytes */ ;

    var_dump(ftell($file_handle));
    var_dump( feof($file_handle) );
    echo "Reading 1024 bytes from file, expecting 1024 bytes ... ";
    $data_from_file=fread($file_handle, 1024);
    check_size($data_from_file,1024);
    var_dump(ftell($file_handle));
    var_dump( feof($file_handle) );
    var_dump( md5($data_from_file) ); // calculate the hash and dump it

    /* read file by giving size more than its size */
    var_dump(rewind($file_handle));
    var_dump(ftell($file_handle));
    var_dump( feof($file_handle) );
    /*reading 1030 bytes from file, expecting 1024 bytes */ ;
    echo "Reading 1030 bytes from file, expecting 1024 bytes ... ";
    $data_from_file=fread($file_handle, 1030);// request for 6 bytes more than its size
    check_size($data_from_file,1024);
    var_dump(ftell($file_handle));
    var_dump( feof($file_handle) );
    var_dump( md5($data_from_file) ); // calculate the hash and dump it

    // reading 1000 bytes within the file max size
    var_dump(rewind($file_handle));
    var_dump(ftell($file_handle));
    var_dump( feof($file_handle) );
    /*reading 1000 bytes from file, expecting 1000 bytes */ ;
    echo "Reading 1000 bytes from file, expecting 1000 bytes ... ";
    $data_from_file=fread($file_handle, 1000);// request for 24 bytes less than its size
    check_size($data_from_file,1000);
    var_dump(ftell($file_handle));
    var_dump( feof($file_handle) );
    var_dump( md5($data_from_file) ); // calculate the hash and dump it
    var_dump(fclose($file_handle)); // now close the file
  } // end of inner for loop

  // delete the file created
  delete_file($filename); // delete file with name
} // end of outer foreach loop
echo "Done\n";
?>
--EXPECT--
*** Testing fread() basic operations ***

-- Testing fread) with file having data of type numeric --
-- File opened in mode r --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
bool(true)
-- File opened in mode rb --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
bool(true)
-- File opened in mode rt --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
bool(true)
-- File opened in mode r+ --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
bool(true)
-- File opened in mode r+b --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
bool(true)
-- File opened in mode r+t --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "4501f99f2b79d0345f26f1394aca58a3"
bool(true)

-- Testing fread) with file having data of type text --
-- File opened in mode r --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
bool(true)
-- File opened in mode rb --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
bool(true)
-- File opened in mode rt --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
bool(true)
-- File opened in mode r+ --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
bool(true)
-- File opened in mode r+b --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
bool(true)
-- File opened in mode r+t --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "e486000c4c8452774f746a27658d87fa"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "e486000c4c8452774f746a27658d87fa"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "2ec76a59f8c44b8f8a0f5139f61bb1bd"
bool(true)

-- Testing fread) with file having data of type text_with_new_line --
-- File opened in mode r --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
bool(true)
-- File opened in mode rb --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
bool(true)
-- File opened in mode rt --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
bool(true)
-- File opened in mode r+ --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
bool(true)
-- File opened in mode r+b --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
bool(true)
-- File opened in mode r+t --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a148fa8110bbac875d84fc9d7056c0a1"
bool(true)

-- Testing fread) with file having data of type alphanumeric --
-- File opened in mode r --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
bool(true)
-- File opened in mode rb --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
bool(true)
-- File opened in mode rt --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
bool(true)
-- File opened in mode r+ --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
bool(true)
-- File opened in mode r+b --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
bool(true)
-- File opened in mode r+t --
int(0)
bool(false)
Reading 1024 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(false)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
bool(true)
int(0)
bool(false)
Reading 1030 bytes from file, expecting 1024 bytes ... OK
int(1024)
bool(true)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
bool(true)
int(0)
bool(false)
Reading 1000 bytes from file, expecting 1000 bytes ... OK
int(1000)
bool(false)
string(32) "a49d752f980184c7f44568e930f89c72"
bool(true)
Done
