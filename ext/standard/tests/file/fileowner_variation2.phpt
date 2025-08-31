--TEST--
Test fileowner() function: usage variations - invalid filenames
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--CONFLICTS--
obscure_filename
--FILE--
<?php

/* Testing fileowner() with invalid arguments -int, float, bool, NULL, resource */

$file_path = __DIR__;

echo "*** Testing Invalid file types ***\n";
$filenames = array(
  /* Invalid filenames */
  -2.34555,
  " ",
  "",
  TRUE,
  FALSE,

  /* scalars */
  1234,
  0
);

/* loop through to test each element the above array */
foreach( $filenames as $filename ) {
  var_dump( fileowner($filename) );
  clearstatcache();
}
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

Warning: fileowner(): stat failed for 1234 in %s on line %d
bool(false)

Warning: fileowner(): stat failed for 0 in %s on line %d
bool(false)
