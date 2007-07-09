--TEST--
Test readlink() and realpath() functions: usage variation - invalid args
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no symlinks on Windows');
}
?>
--FILE--
<?php
/* Prototype: string readlink ( string $path );
   Description: Returns the target of a symbolic link

   Prototype: string realpath ( string $path );
   Description: Returns canonicalized absolute pathname
*/

echo "*** Testing readlink() and realpath() : usage variations ***\n";
  
echo "\n*** Testing readlink() and realpath() with linkname as empty string, NULL and single space ***\n";
$link_string = array (
  /* linkname as spaces */
  " ",
  ' ',

  /* empty linkname */
  "",
  '',
  NULL,
  null
 );
for($loop_counter = 0; $loop_counter < count($link_string); $loop_counter++) {
  echo "-- Iteration";
  echo $loop_counter + 1;
  echo " --\n";

  var_dump( readlink($link_string[$loop_counter]) ); 
  var_dump( realpath($link_string[$loop_counter]) ); 
}

echo "Done\n";
?>
--CLEAN--
<?php
$name_prefix = dirname(__FILE__)."/readlink_realpath_variation2";
unlink("$name_prefix/home/tests/link/readlink_realpath_variation2.tmp");
rmdir("$name_prefix/home/tests/link/");
rmdir("$name_prefix/home/tests/");
rmdir("$name_prefix/home/");
rmdir("$name_prefix/");
?>
--EXPECTF--
*** Testing readlink() and realpath() : usage variations ***

*** Testing readlink() and realpath() with linkname as empty string, NULL and single space ***
-- Iteration1 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
%s
-- Iteration2 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
%s
-- Iteration3 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
string(%d) "%s"
-- Iteration4 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
string(%d) "%s"
-- Iteration5 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
string(%d) "%s"
-- Iteration6 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
string(%d) "%s"
Done
