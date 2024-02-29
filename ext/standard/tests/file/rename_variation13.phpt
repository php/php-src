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

Warning: rename('-1', '%s'): No such file or directory in %s on line %d
bool(false)
-- testing '1' --
bool(true)

Warning: rename('1', '%s'): No such file or directory in %s on line %d
bool(false)
-- testing '' --

Warning: rename('%s', ''): Invalid argument in %s on line %d
bool(false)

Warning: rename('', '%s'): Invalid argument in %s on line %d
bool(false)
-- testing '' --

Warning: rename('%s', ''): Invalid argument in %s on line %d
bool(false)

Warning: rename('', '%s'): Invalid argument in %s on line %d
bool(false)
-- testing ' ' --
bool(true)

Warning: rename(' ', '%s'): No such file or directory in %s on line %d
bool(false)
-- testing '/no/such/file/dir' --

Warning: rename('%s'): No such file or directory in %s on line %d
bool(false)

Warning: rename('%s'): No such file or directory in %s on line %d
bool(false)
-- testing 'php/php' --

Warning: rename('%s', 'php/php'): No such file or directory in %s on line %d
bool(false)

Warning: rename('php/php', '%s'): No such file or directory in %s on line %d
bool(false)
