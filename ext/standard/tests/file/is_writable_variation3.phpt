--TEST--
Test is_writable() and its alias is_writeable() function: usage variations - invalid file names
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  // Skip if being run by root (files are always readable, writeable and executable)
  $filename = dirname(__FILE__)."/is_writable_root_check.tmp";
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
/* Prototype: bool is_writable ( string $filename );
   Description: Tells whether the filename is writable.

   is_writeable() is an alias of is_writable()
*/

/* test is_writable() & is_writeable() with invalid arguments */

echo "*** Testing is_writable(): usage variations ***\n";

echo "\n*** Testing is_writable() with invalid filenames ***\n";
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
   is a writable file */
foreach( $misc_files as $misc_file ) {
  var_dump( is_writable($misc_file) );
  var_dump( is_writeable($misc_file) );
  clearstatcache();
}

echo "Done\n";
?>
--EXPECTF--
*** Testing is_writable(): usage variations ***

*** Testing is_writable() with invalid filenames ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

Warning: is_writable() expects parameter 1 to be a valid path, array given in %s on line %d
NULL

Warning: is_writeable() expects parameter 1 to be a valid path, array given in %s on line %d
NULL
bool(false)
bool(false)
Done
