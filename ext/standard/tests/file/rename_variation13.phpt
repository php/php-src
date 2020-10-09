--TEST--
Test rename() function : variation - various invalid paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip. Not for Windows");
?>
--CONFLICTS--
obscure_filename
--FILE--
<?php
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
?>
--EXPECTF--
*** Testing rename() with obscure files ***
-- testing '-1' --
bool(true)

Warning: rename(-1,%s/renameVar13/afile.tmp): No such file or directory in %s on line %d
bool(false)
-- testing '1' --
bool(true)

Warning: rename(1,%s/renameVar13/afile.tmp): No such file or directory in %s on line %d
bool(false)
-- testing '' --

Warning: rename(%s/renameVar13/afile.tmp,): %s in %s on line %d
bool(false)

Warning: rename(,%s/renameVar13/afile.tmp): %s in %s on line %d
bool(false)
-- testing '' --

Warning: rename(%s/renameVar13/afile.tmp,): %s in %s on line %d
bool(false)

Warning: rename(,%s/renameVar13/afile.tmp): %s in %s on line %d
bool(false)
-- testing '' --

Warning: rename(%s/renameVar13/afile.tmp,): %s in %s on line %d
bool(false)

Warning: rename(,%s/renameVar13/afile.tmp): %s in %s on line %d
bool(false)
-- testing ' ' --
bool(true)

Warning: rename( ,%s/renameVar13/afile.tmp): No such file or directory in %s on line %d
bool(false)
-- testing '/no/such/file/dir' --

Warning: rename(%s/renameVar13/afile.tmp,/no/such/file/dir): No such file or directory in %s on line %d
bool(false)

Warning: rename(/no/such/file/dir,%s/renameVar13/afile.tmp): No such file or directory in %s on line %d
bool(false)
-- testing 'php/php' --

Warning: rename(%s/renameVar13/afile.tmp,php/php): %s directory in %s on line %d
bool(false)

Warning: rename(php/php,%s/renameVar13/afile.tmp): %s directory in %s on line %d
bool(false)
