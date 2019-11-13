--TEST--
Test readlink() function: usage variations - invalid filenames
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--CONFLICTS--
obscure_filename
--FILE--
<?php
/* Prototype: string readlink ( string $path );
   Description: Returns the target of a symbolic link */

/* Testing readlink() with invalid arguments -int, float, bool, NULL, resource */

$file_path = __DIR__;
$file_handle = fopen($file_path."/readlink_variation2.tmp", "w");

echo "*** Testing Invalid file types ***\n";
$filenames = array(
  /* Invalid filenames */
  -2.34555,
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
  var_dump( readlink($filename) );
  clearstatcache();
}
fclose($file_handle);
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/readlink_variation2.tmp");
?>
--EXPECTF--
*** Testing Invalid file types ***

Warning: readlink(): %s in %s on line %d
bool(false)

Warning: readlink(): %s in %s on line %d
bool(false)

Warning: readlink(): %s in %s on line %d
bool(false)

Warning: readlink(): %s in %s on line %d
bool(false)

Warning: readlink(): %s in %s on line %d
bool(false)

Warning: readlink() expects parameter 1 to be a valid path, resource given in %s on line %d
NULL

Warning: readlink(): %s in %s on line %d
bool(false)

Warning: readlink(): %s in %s on line %d
bool(false)
