--TEST--
Test readlink() function: usage variations - invalid filenames
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no links on Windows');
}
?>
--FILE--
<?php
/* Prototype: string readlink ( string $path );
   Description: Returns the target of a symbolic link */

/* Testing readlink() with invalid arguments -int, float, bool, NULL, resource */

$file_path = dirname(__FILE__);
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

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
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

Warning: readlink() expects parameter 1 to be string, resource given in %s on line %d
NULL

Warning: readlink(): %s in %s on line %d
bool(false)

Warning: readlink(): %s in %s on line %d
bool(false)

*** Done ***

