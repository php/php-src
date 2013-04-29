--TEST--
Test is_file() function: usage variations - invalid filenames
--FILE--
<?php
/* Prototype: bool is_file ( string $filename );
   Description: Tells whether the filename is a regular file
     Returns TRUE if the filename exists and is a regular file
*/

/* Testing is_file() with invalid arguments -int, float, bool, NULL, resource */

$file_path = dirname(__FILE__);
$file_handle = fopen($file_path."/is_file_variation3.tmp", "w");

echo "*** Testing Invalid file types ***\n";
$filenames = array(
  /* Invalid filenames */
  -2.34555,
  " ",
  "",
  TRUE,
  FALSE,
  NULL,
  $file_handle,
  
  /* scalars */
  1234,
  0
);
   
/* loop through to test each element the above array */
foreach( $filenames as $filename ) {
  var_dump( is_file($filename) );
  clearstatcache();
}
fclose($file_handle);

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/is_file_variation3.tmp");
?>
--EXPECTF--
*** Testing Invalid file types ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

Warning: is_file() expects parameter 1 to be a valid path, resource given in %s on line %d
NULL
bool(false)
bool(false)

*** Done ***

