--TEST--
Test file_get_contents() function : variation - obscure filenames
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
?>
--CONFLICTS--
obscure_filename
--FILE--
<?php
echo "*** Testing file_get_contents() : variation ***\n";
/* An array of filenames */
$names_arr = array(
  /* Invalid args */
  -1,
  TRUE,
  FALSE,
  NULL,
  "",
  " ",
  "\0",
  array(),

  /* prefix with path separator of a non existing directory*/
  "/no/such/file/dir",
  "php/php"

);

for( $i=0; $i<count($names_arr); $i++ ) {
    echo "-- Iteration $i --\n";
    try {
        var_dump(file_get_contents($names_arr[$i]));
    } catch (\TypeError|\ValueError $e) {
        echo get_class($e) . ': ' . $e->getMessage(), "\n";
    }
}

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing file_get_contents() : variation ***
-- Iteration 0 --

Warning: file_get_contents(-1): Failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 1 --

Warning: file_get_contents(1): Failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 2 --
ValueError: Path cannot be empty
-- Iteration 3 --
ValueError: Path cannot be empty
-- Iteration 4 --
ValueError: Path cannot be empty
-- Iteration 5 --

Warning: file_get_contents( ): Failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 6 --
ValueError: file_get_contents(): Argument #1 ($filename) must not contain any null bytes
-- Iteration 7 --
TypeError: file_get_contents(): Argument #1 ($filename) must be of type string, array given
-- Iteration 8 --

Warning: file_get_contents(/no/such/file/dir): Failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 9 --

Warning: file_get_contents(php/php): Failed to open stream: No such file or directory in %s on line %d
bool(false)

*** Done ***
