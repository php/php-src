--TEST--
Test rename() function : variation - various invalid paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip. Not for Windows");
?>
--FILE--
<?php
/* Prototype  : bool rename(string old_name, string new_name[, resource context])
 * Description: Rename a file
 * Source code: ext/standard/file.c
 * Alias to functions:
 */

echo "*** Testing rename() with obscure files ***\n";
$file_path = __DIR__."/renameVar13";
$aFile = $file_path.'/afile.tmp';

mkdir($file_path);

/* An array of files */
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
  $name = $names_arr[$i];
  echo @"-- testing '$name' --\n";
  touch($aFile);
  var_dump(rename($aFile, $name));
  if (file_exists($name)) {
     unlink($name);
  }
  if (file_exists($aFile)) {
     unlink($aFile);
  }
  var_dump(rename($name, $aFile));
  if (file_exists($aFile)) {
     unlink($aFile);
  }
}

rmdir($file_path);
echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing rename() with obscure files ***
-- testing '-1' --
bool(true)

Warning: rename(): No such file or directory (-1) in %s on line %d
bool(false)
-- testing '1' --
bool(true)

Warning: rename(): No such file or directory (1) in %s on line %d
bool(false)
-- testing '' --

Warning: rename(): %r(Invalid argument|No such file or directory)%r (%s/renameVar13/afile.tmp) in %s on line %d
bool(false)

Warning: rename(): %r(Invalid argument|No such file or directory)%r () in %s on line %d
bool(false)
-- testing '' --

Warning: rename(): %r(Invalid argument|No such file or directory)%r (%s/renameVar13/afile.tmp) in %s on line %d
bool(false)

Warning: rename(): %r(Invalid argument|No such file or directory)%r () in %s on line %d
bool(false)
-- testing '' --

Warning: rename(): %r(Invalid argument|No such file or directory)%r (%s/renameVar13/afile.tmp) in %s on line %d
bool(false)

Warning: rename(): %r(Invalid argument|No such file or directory)%r () in %s on line %d
bool(false)
-- testing ' ' --
bool(true)

Warning: rename(): No such file or directory ( ) in %s on line %d
bool(false)
-- testing '%s' --

Warning: rename() expects parameter 2 to be a valid path, string given in %s on line %d
bool(false)

Warning: file_exists() expects parameter 1 to be a valid path, string given in %s on line %d

Warning: rename() expects parameter 1 to be a valid path, string given in %s on line %d
bool(false)
-- testing 'Array' --

Warning: rename() expects parameter 2 to be a valid path, array given in %s on line %d
bool(false)

Warning: file_exists() expects parameter 1 to be a valid path, array given in %s on line %d

Warning: rename() expects parameter 1 to be a valid path, array given in %s on line %d
bool(false)
-- testing '/no/such/file/dir' --

Warning: rename(): No such file or directory (%s/renameVar13/afile.tmp) in %s on line %d
bool(false)

Warning: rename(): No such file or directory (/no/such/file/dir) in %s on line %d
bool(false)
-- testing 'php/php' --

Warning: rename(): No such file or directory (%s/renameVar13/afile.tmp) in %s on line %d
bool(false)

Warning: rename(): No such file or directory (php/php) in %s on line %d
bool(false)

*** Done ***
