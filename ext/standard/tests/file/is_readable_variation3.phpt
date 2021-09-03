--TEST--
Test is_readable() function: usage variations - invalid file names
--SKIPIF--
<?php
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
/* test is_executable() with invalid arguments */

echo "*** Testing is_readable(): usage variations ***\n";

echo "\n*** Testing is_readable() on miscellaneous filenames ***\n";
$misc_files = array(
  0,
  1234,
  -2.34555,
  TRUE,
  FALSE,
  " ",
);
/* loop through to test each element in the above array
   is a readable file */
foreach( $misc_files as $misc_file ) {
  var_dump( is_readable($misc_file) );
  clearstatcache();
}

echo "Done\n";
?>
--EXPECT--
*** Testing is_readable(): usage variations ***

*** Testing is_readable() on miscellaneous filenames ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Done
