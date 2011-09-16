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
--FILE--
<?php
/* Prototype  : string file_get_contents(string filename [, bool use_include_path [, resource context [, long offset [, long maxlen]]]])
 * Description: Read the entire file into a string 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */

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
  var_dump(file_get_contents($names_arr[$i]));
}

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing file_get_contents() : variation ***
-- Iteration 0 --

Warning: file_get_contents(-1): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 1 --

Warning: file_get_contents(1): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 2 --

Warning: file_get_contents(): Filename cannot be empty in %s on line %d
bool(false)
-- Iteration 3 --

Warning: file_get_contents(): Filename cannot be empty in %s on line %d
bool(false)
-- Iteration 4 --

Warning: file_get_contents(): Filename cannot be empty in %s on line %d
bool(false)
-- Iteration 5 --

Warning: file_get_contents( ): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 6 --

Warning: file_get_contents() expects parameter 1 to be a valid path, string given in %s on line %d
NULL
-- Iteration 7 --

Warning: file_get_contents() expects parameter 1 to be a valid path, array given in %s on line %d
NULL
-- Iteration 8 --

Warning: file_get_contents(/no/such/file/dir): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 9 --

Warning: file_get_contents(php/php): failed to open stream: No such file or directory in %s on line %d
bool(false)

*** Done ***
