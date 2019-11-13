--TEST--
Test fileowner() function: usage variations - invalid filenames
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--CONFLICTS--
obscure_filename
--FILE--
<?php
/* Prototype: int fileowner ( string $filename )
 * Description: Returns the user ID of the owner of the file, or
 *              FALSE in case of an error.
 */

/* Testing fileowner() with invalid arguments -int, float, bool, NULL, resource */

$file_path = __DIR__;
$file_handle = fopen($file_path."/fileowner_variation2.tmp", "w");

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
  var_dump( fileowner($filename) );
  clearstatcache();
}
fclose($file_handle);
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/fileowner_variation2.tmp");
?>
--EXPECTF--
*** Testing Invalid file types ***

Warning: fileowner(): stat failed for -2.34555 in %s on line %d
bool(false)

Warning: fileowner(): stat failed for   in %s on line %d
bool(false)
bool(false)

Warning: fileowner(): stat failed for 1 in %s on line %d
bool(false)
bool(false)
bool(false)

Warning: fileowner() expects parameter 1 to be a valid path, resource given in %s on line %d
NULL

Warning: fileowner(): stat failed for 1234 in %s on line %d
bool(false)

Warning: fileowner(): stat failed for 0 in %s on line %d
bool(false)
