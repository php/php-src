--TEST--
Test is_readable() function: usage variations - invalid file names
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {

  // Skip if being run by root (files are always readable, writeable and executable)
  $filename = dirname(__FILE__)."/is_readable_root_check.tmp";
  $fp = fopen($filename, 'w');
  fclose($fp);
  if(fileowner($filename) == 0) {
        unlink ($filename);
        die('skip cannot be run as root');
  }
  unlink($filename);
}
?>
--FILE--
<?php
/* Prototype: bool is_readable ( string $filename );
   Description: Tells whether the filename is readable.
*/

/* test is_executable() with invalid arguments */

echo "*** Testing is_readable(): usage variations ***\n";

$file_handle = fopen(__FILE__, "r");
unset($file_handle);

echo "\n*** Testing is_readable() on miscelleneous filenames ***\n";
$misc_files = array(
  0,
  1234,
  -2.34555,
  TRUE,
  FALSE,
  NULL,
  " ",
  @array(),
  @$file_handle
);
/* loop through to test each element in the above array 
   is a readable file */
foreach( $misc_files as $misc_file ) {
  var_dump( is_readable($misc_file) );
  clearstatcache();
}

echo "Done\n";
?>
--EXPECTF--
*** Testing is_readable(): usage variations ***

*** Testing is_readable() on miscelleneous filenames ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

Warning: is_readable() expects parameter 1 to be string, array given in %s on line %d
NULL
bool(false)
Done

