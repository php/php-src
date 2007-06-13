--TEST--
Test fwrite() function : usage variations
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
 Test fwrite with file opened in mode : a,ab,at,a+,a+b,a+t,r,rb,rt,r+,r+b,r+t
 File having content of type numeric, text,text_with_new_line & alphanumeric
*/

$file_modes = array("r","rb","rt","r+", "r+b", "r+t","a","ab","at","a+","a+b",
                    "a+t","x","xb","xt","x+","x+b","x+t");
$file_content_types = array("numeric","text","text_with_new_line","alphanumeric");


foreach($file_content_types as $file_content_type) {
  echo "\n-- Testing fwrite() with file having content of type ". $file_content_type ." --\n";

  /* open the file using $files_modes and perform fwrite() on it */
  foreach($file_modes as $file_mode) {
    echo "-- Opening file in $file_mode --\n";

    $filename = dirname(__FILE__)."/fwrite_variation-win321.tmp"; // this is name of the file
    if ( ! strstr( $file_mode, "x" ) ) {
      create_files ( dirname(__FILE__), 1, $file_content_type, 0755, 1, "w", "fwrite_variation-win32");
    }

    $file_handle = fopen($filename, $file_mode);
    if(!$file_handle) {
      echo "Error: failed to fopen() file: $filename!";
      exit();
    }

    $data_to_be_written="";
    fill_buffer($data_to_be_written,$file_content_type,1024);  //get the data of size 1024

    /*  Write the in to the file, verify it by checking the file pointer position, eof position, 
        filesize & by displaying the content */
    switch($file_mode) {
      case "r":
      case "rb":
      case "rt":
        var_dump( ftell($file_handle) );  // expected: 0
        var_dump( fwrite($file_handle, $data_to_be_written )); 
        var_dump( ftell($file_handle) );  // expected: 0
        var_dump( feof($file_handle) );  // expected: false 
  
        // move the file pointer to end of the file and try fwrite()
        fseek($file_handle, SEEK_END, 0);
        var_dump( ftell($file_handle) );  // expecting 1024
        var_dump( fwrite($file_handle, $data_to_be_written) ); // fwrite to fail
        var_dump( ftell($file_handle) );  //check that file pointer points at eof, expected: 1024
        var_dump( feof($file_handle) );  // ensure that  feof() points to eof, expected: true

    	// ensure that file content/size didn't change.
        var_dump( fclose($file_handle) );
        clearstatcache();//clears file status cache
        var_dump( filesize($filename) );  // expected: 1024
        var_dump(md5(file_get_contents($filename))); // hash the output
  	delete_file($filename); // delete file with name fwrite_variation1.tmp
     	break;

      case "r+b":
      case "r+":
      case "r+t":
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
        delete_file($filename); // delete file with name fwrite_variation1.tmp
        break;

      case "a":
      case "ab":
      case "at":
      case "a+":
      case "a+b":
      case "a+t":
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
        delete_file($filename); // delete file with name fwrite_variation.tmp
        break;

      case "x":
      case "xb":
      case "xt":
      case "x+":
      case "x+b":
      case "x+t":
        // write data to the file
        var_dump( ftell($file_handle) );
        var_dump( fwrite($file_handle,$data_to_be_written,400));
        var_dump( ftell($file_handle) );
        var_dump( feof($file_handle) );  // expected: true

        //check the filesize and content
        // close the file, get the size and content of the file.
        var_dump( fclose($file_handle) );
        clearstatcache();//clears file status cache
        var_dump( filesize($filename) );
        var_dump(md5(file_get_contents($filename)));
        // delete the file created
        delete_file($filename); // delete file with name fwrite_variation.tmp
        break;
    } //end of switch
  } // end of inner foreach loop
} // end of outer foreach loop

echo "Done\n";
?>
--EXPECTF--
*** Testing fwrite() various  operations ***

-- Testing fwrite() with file having content of type numeric --
-- Opening file in r --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Opening file in rb --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
-- Opening file in rt --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "950b7457d1deb6332f2fc5d42f3129d6"
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
-- Opening file in x --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "f255efe87ebdf755e515868cea9ad24b"
-- Opening file in xb --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "f255efe87ebdf755e515868cea9ad24b"
-- Opening file in xt --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "f255efe87ebdf755e515868cea9ad24b"
-- Opening file in x+ --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "f255efe87ebdf755e515868cea9ad24b"
-- Opening file in x+b --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "f255efe87ebdf755e515868cea9ad24b"
-- Opening file in x+t --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "f255efe87ebdf755e515868cea9ad24b"

-- Testing fwrite() with file having content of type text --
-- Opening file in r --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Opening file in rb --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "e486000c4c8452774f746a27658d87fa"
-- Opening file in rt --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "e486000c4c8452774f746a27658d87fa"
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
-- Opening file in x --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "c2244282eeca7c2d32d0dacf21e19432"
-- Opening file in xb --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "c2244282eeca7c2d32d0dacf21e19432"
-- Opening file in xt --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "c2244282eeca7c2d32d0dacf21e19432"
-- Opening file in x+ --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "c2244282eeca7c2d32d0dacf21e19432"
-- Opening file in x+b --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "c2244282eeca7c2d32d0dacf21e19432"
-- Opening file in x+t --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "c2244282eeca7c2d32d0dacf21e19432"

-- Testing fwrite() with file having content of type text_with_new_line --
-- Opening file in r --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Opening file in rb --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
-- Opening file in rt --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "b09c8026a64a88d36d4c2f17983964bb"
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
int(1690)
string(32) "656648355b64df6fded53b12fb355ab8"
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
int(1690)
string(32) "656648355b64df6fded53b12fb355ab8"
-- Opening file in x --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "fa6c79b925c2fc754b9d063c6de1d8df"
-- Opening file in xb --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "fa6c79b925c2fc754b9d063c6de1d8df"
-- Opening file in xt --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(444)
string(32) "c96531f6b4c8d9e829c25b87f96ea86e"
-- Opening file in x+ --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "fa6c79b925c2fc754b9d063c6de1d8df"
-- Opening file in x+b --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "fa6c79b925c2fc754b9d063c6de1d8df"
-- Opening file in x+t --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(444)
string(32) "c96531f6b4c8d9e829c25b87f96ea86e"

-- Testing fwrite() with file having content of type alphanumeric --
-- Opening file in r --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
-- Opening file in rb --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
-- Opening file in rt --
int(0)
int(0)
int(0)
bool(false)
int(2)
int(0)
int(2)
bool(false)
bool(true)
int(1024)
string(32) "3fabd48d8eaa65c14e0d93d6880c560c"
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
-- Opening file in x --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "b2a123e1d84e6a03c8520aff7689219e"
-- Opening file in xb --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "b2a123e1d84e6a03c8520aff7689219e"
-- Opening file in xt --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "b2a123e1d84e6a03c8520aff7689219e"
-- Opening file in x+ --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "b2a123e1d84e6a03c8520aff7689219e"
-- Opening file in x+b --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "b2a123e1d84e6a03c8520aff7689219e"
-- Opening file in x+t --
int(0)
int(400)
int(400)
bool(false)
bool(true)
int(400)
string(32) "b2a123e1d84e6a03c8520aff7689219e"
Done