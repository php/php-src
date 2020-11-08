--TEST--
Test is_executable() function: usage variations - invalid file names
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip not for windows');
}
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
/* test is_executable() with invalid arguments */

echo "*** Testing is_executable(): usage variations ***\n";

$file_handle = fopen(__FILE__, "r");
unset($file_handle);

echo "\n*** Testing is_executable() on invalid files ***\n";
$invalid_files = array(
  0,
  1234,
  -2.34555,
  TRUE,
  FALSE,
  NULL,
  " ",
  @$file_handle
);
/* loop through to test each element in the above array
   is an executable file */
foreach( $invalid_files as $invalid_file ) {
  var_dump( is_executable($invalid_file) );
  clearstatcache();
}

echo "Done\n";
?>
--EXPECT--
*** Testing is_executable(): usage variations ***

*** Testing is_executable() on invalid files ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Done
