--TEST--
Test fgets() function : usage variations - write only modes (Bug #42036)
--FILE--
<?php
/*
 Prototype: string fgets ( resource $handle [, int $length] );
 Description: Gets a line from file pointer
*/
/* try fgets on files which are opened in non readable modes
    w, wb, wt,
    a, ab, at,
    x, xb, xt
*/
// include the header for common test function
include ("file.inc");

echo "*** Testing fgets() with file opened in write only mode ***\n";

$file_modes = array("w", "wb", "wt", "a", "ab", "at", "x", "xb", "xt");
$filename = dirname(__FILE__)."/fgets_variation1.tmp";
foreach ($file_modes as $file_mode ) {
  echo "-- File opened in mode : $file_mode --\n";

  $file_handle = fopen($filename, $file_mode);
  if(!$file_handle) {
    echo "Error: failed to open file $filename!\n";
    exit();
  }
  $data = "fgets_variation test";
  fwrite($file_handle, $data);

  // rewind the file pointer to beginning of the file
  var_dump( rewind($file_handle) );
  var_dump( ftell($file_handle) );
  var_dump( feof($file_handle) );

  // read from file
  var_dump( fgets($file_handle) ); // expected : no chars should be read
  var_dump( ftell($file_handle) ); // ensure that file pointer position is not changed
  var_dump( feof($file_handle) ); // check if end of file pointer is set

  // close the file
  fclose($file_handle);

  // delete the file
  unlink($filename);
}

echo "Done\n";
?>
--EXPECTF--
*** Testing fgets() with file opened in write only mode ***
-- File opened in mode : w --
bool(true)
int(0)
bool(false)
bool(false)
int(0)
bool(false)
-- File opened in mode : wb --
bool(true)
int(0)
bool(false)
bool(false)
int(0)
bool(false)
-- File opened in mode : wt --
bool(true)
int(0)
bool(false)
bool(false)
int(0)
bool(false)
-- File opened in mode : a --
bool(true)
int(0)
bool(false)
bool(false)
int(0)
bool(false)
-- File opened in mode : ab --
bool(true)
int(0)
bool(false)
bool(false)
int(0)
bool(false)
-- File opened in mode : at --
bool(true)
int(0)
bool(false)
bool(false)
int(0)
bool(false)
-- File opened in mode : x --
bool(true)
int(0)
bool(false)
bool(false)
int(0)
bool(false)
-- File opened in mode : xb --
bool(true)
int(0)
bool(false)
bool(false)
int(0)
bool(false)
-- File opened in mode : xt --
bool(true)
int(0)
bool(false)
bool(false)
int(0)
bool(false)
Done
