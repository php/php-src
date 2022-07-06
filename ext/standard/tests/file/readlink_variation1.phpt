--TEST--
Test readlink() function: usage variations - invalid filenames
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--CONFLICTS--
obscure_filename
--FILE--
<?php
/* Testing readlink() with invalid arguments -int, float, bool, NULL, resource */

$file_path = __DIR__;

echo "*** Testing Invalid file types ***\n";
$filenames = array(
  /* Invalid filenames */
  -2.34555,
  "",
  TRUE,
  FALSE,
  NULL,

  /* scalars */
  1234,
  0
);

/* loop through to test each element the above array */
foreach( $filenames as $filename ) {
  var_dump( readlink($filename) );
  clearstatcache();
}
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

Warning: readlink(): %s in %s on line %d
bool(false)

Warning: readlink(): %s in %s on line %d
bool(false)
