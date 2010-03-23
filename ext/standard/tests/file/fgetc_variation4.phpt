--TEST--
Test fgetc() function : usage variations - different read modes 
--FILE--
<?php
/*
 Prototype: string fgetc ( resource $handle );
 Description: Gets character from file pointer
*/

/* read from fie using fgetc, file opened using different
   read read modes */

echo "*** Testing fgetc() : usage variations ***\n";
echo "-- Testing fgetc() with files opened with different read modes --\n";

$file_modes = array( "a+", "a+b", "a+t", 
                     "x+", "x+b", "x+t", 
                     "w+", "w+b", "w+t" );

$filename = dirname(__FILE__)."/fgetc_variation4.tmp";
foreach ($file_modes as $file_mode ) {
  echo "-- File opened in mode : $file_mode --\n";

  $file_handle = fopen($filename, $file_mode);
  if(!$file_handle) {
    echo "Error: failed to open file $filename!\n";
    exit();
  }
  $data = "fgetc\n test";
  fwrite($file_handle, $data);

  // rewind the file pointer to begining of the file
  var_dump( rewind($file_handle) ); 
  var_dump( ftell($file_handle) ); 
  var_dump( feof($file_handle) );

  // read from file, at least 7 chars
  for($counter =0; $counter < 7; $counter ++) {
    var_dump( fgetc($file_handle) ); // expected : 1 char
    var_dump( ftell($file_handle) );
    var_dump( feof($file_handle) ); // check if end of file pointer is set
  }

  // close the file
  fclose($file_handle);

  // delete the file
  unlink($filename); 
}

echo "Done\n";
?>
--EXPECTF--
 *** Testing fgetc() : usage variations ***
-- Testing fgetc() with files opened with different read modes --
-- File opened in mode : a+ --
bool(true)
int(0)
bool(false)
string(1) "f"
int(1)
bool(false)
string(1) "g"
int(2)
bool(false)
string(1) "e"
int(3)
bool(false)
string(1) "t"
int(4)
bool(false)
string(1) "c"
int(5)
bool(false)
string(1) "
"
int(6)
bool(false)
string(1) " "
int(7)
bool(false)
-- File opened in mode : a+b --
bool(true)
int(0)
bool(false)
string(1) "f"
int(1)
bool(false)
string(1) "g"
int(2)
bool(false)
string(1) "e"
int(3)
bool(false)
string(1) "t"
int(4)
bool(false)
string(1) "c"
int(5)
bool(false)
string(1) "
"
int(6)
bool(false)
string(1) " "
int(7)
bool(false)
-- File opened in mode : a+t --
bool(true)
int(0)
bool(false)
string(1) "f"
int(1)
bool(false)
string(1) "g"
int(2)
bool(false)
string(1) "e"
int(3)
bool(false)
string(1) "t"
int(4)
bool(false)
string(1) "c"
int(5)
bool(false)
string(1) "
"
int(6)
bool(false)
string(1) " "
int(7)
bool(false)
-- File opened in mode : x+ --
bool(true)
int(0)
bool(false)
string(1) "f"
int(1)
bool(false)
string(1) "g"
int(2)
bool(false)
string(1) "e"
int(3)
bool(false)
string(1) "t"
int(4)
bool(false)
string(1) "c"
int(5)
bool(false)
string(1) "
"
int(6)
bool(false)
string(1) " "
int(7)
bool(false)
-- File opened in mode : x+b --
bool(true)
int(0)
bool(false)
string(1) "f"
int(1)
bool(false)
string(1) "g"
int(2)
bool(false)
string(1) "e"
int(3)
bool(false)
string(1) "t"
int(4)
bool(false)
string(1) "c"
int(5)
bool(false)
string(1) "
"
int(6)
bool(false)
string(1) " "
int(7)
bool(false)
-- File opened in mode : x+t --
bool(true)
int(0)
bool(false)
string(1) "f"
int(1)
bool(false)
string(1) "g"
int(2)
bool(false)
string(1) "e"
int(3)
bool(false)
string(1) "t"
int(4)
bool(false)
string(1) "c"
int(5)
bool(false)
string(1) "
"
int(6)
bool(false)
string(1) " "
int(7)
bool(false)
-- File opened in mode : w+ --
bool(true)
int(0)
bool(false)
string(1) "f"
int(1)
bool(false)
string(1) "g"
int(2)
bool(false)
string(1) "e"
int(3)
bool(false)
string(1) "t"
int(4)
bool(false)
string(1) "c"
int(5)
bool(false)
string(1) "
"
int(6)
bool(false)
string(1) " "
int(7)
bool(false)
-- File opened in mode : w+b --
bool(true)
int(0)
bool(false)
string(1) "f"
int(1)
bool(false)
string(1) "g"
int(2)
bool(false)
string(1) "e"
int(3)
bool(false)
string(1) "t"
int(4)
bool(false)
string(1) "c"
int(5)
bool(false)
string(1) "
"
int(6)
bool(false)
string(1) " "
int(7)
bool(false)
-- File opened in mode : w+t --
bool(true)
int(0)
bool(false)
string(1) "f"
int(1)
bool(false)
string(1) "g"
int(2)
bool(false)
string(1) "e"
int(3)
bool(false)
string(1) "t"
int(4)
bool(false)
string(1) "c"
int(5)
bool(false)
string(1) "
"
int(6)
bool(false)
string(1) " "
int(7)
bool(false)
Done