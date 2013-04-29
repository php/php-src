--TEST--
Test fileperms() function: usage variations - invalid filenames
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype: int fileperms ( string $filename )
 * Description: Returns the group ID of the file, or FALSE in case of an error.
 */

/* Testing fileperms() with invalid arguments -int, float, bool, NULL, resource */

$file_path = dirname(__FILE__);
$file_handle = fopen($file_path."/fileperms_variation2.tmp", "w");

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
  var_dump( fileperms($filename) );
  clearstatcache();
}
fclose($file_handle);

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/fileperms_variation2.tmp");
?>
--EXPECTF--
*** Testing Invalid file types ***

Warning: fileperms(): stat failed for -2.34555 in %s on line %d
bool(false)

Warning: fileperms(): stat failed for   in %s on line %d
bool(false)
bool(false)

Warning: fileperms(): stat failed for 1 in %s on line %d
bool(false)
bool(false)
bool(false)

Warning: fileperms() expects parameter 1 to be a valid path, resource given in %s on line %d
NULL

Warning: fileperms(): stat failed for 1234 in %s on line %d
bool(false)

Warning: fileperms(): stat failed for 0 in %s on line %d
bool(false)

*** Done ***

