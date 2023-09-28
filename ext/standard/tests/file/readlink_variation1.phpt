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
--EXPECTF--
*** Testing Invalid file types ***

Warning: readlink('-2.34555'): No such file or directory in %s on line %d
bool(false)

Warning: readlink(''): No such file or directory in %s on line %d
bool(false)

Warning: readlink('1'): No such file or directory in %s on line %d
bool(false)

Warning: readlink(''): No such file or directory in %s on line %d
bool(false)

Warning: readlink('1234'): No such file or directory in %s on line %d
bool(false)

Warning: readlink('0'): No such file or directory in %s on line %d
bool(false)
